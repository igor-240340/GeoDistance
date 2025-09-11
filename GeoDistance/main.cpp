#include <iostream>
#include <format>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Core.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Vec3f.h"
#include "Vec4f.h"
#include "Mat4f.h"

#include "graphics.h"

#include "geopos.h"

#ifdef _DEBUG
#include "test.h"
#endif // _DEBUG

struct TransformMatrices {
	Mat4f camera;

	// Обновляются из UI.
	float horiz_angle_rad;
	float vert_angle;

	Mat4f horiz_rot;
	Mat4f vert_rot;
	Mat4f translate_z;

	// Перестраиваются, когда пользователь изменил хотя бы одно значение в UI.
	Mat4f yaw_trans_cam;
	Mat4f tilt_yaw_trans_cam;
	Mat4f pitch_roll;

	// Устанавливается в true, когда пользователь поменял в UI хотя бы одно значение.
	// Это нужно, чтобы не перестраивать матрицы в каждом кадре.
	// При первом запуске структура инициализируется новыми значениями, поэтому true.
	bool changed = true;
};

void build_ui(tgui::Gui& gui, TransformMatrices& transforms);
void init_transforms(TransformMatrices& transforms);
void load_model(std::string model_path, std::vector<Polygon>& polygons);
void draw_globe(std::vector<Polygon> globe_mesh, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms);
void rasterize_polygons_flat_shaded_ortho(const std::vector<Polygon>& polygons, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const Mat4f& transform);

int main() {
#ifdef _DEBUG
	run_test();
#endif // _DEBUG

	constexpr int w = 800;
	constexpr int h = 600;

	sf::RenderWindow window(sf::VideoMode(w, h), "GeoDistance Demo", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(0);
	window.setVerticalSyncEnabled(true);

	tgui::Gui gui{ window };

	TransformMatrices transforms;
	init_transforms(transforms);
	build_ui(gui, transforms);

	sf::Texture texture;
	if (!texture.create(w, h)) {
		std::cout << "SFML: texture.create() failed.\n";
		return 1;
	}
	sf::Sprite sprite(texture);

	Light light{ Vec3f{0.0f, 0.0f, -1.0f} };
	Framebuffer framebuffer{ w, h, std::vector<sf::Uint8>(w * h * 4) };
	ZBuffer z_buffer{ w, h, std::vector<float>(w * h) };

	// Читаем меш глобуса.
	std::vector<Polygon> globe_mesh;
	load_model(std::format("assets/globe/globe.obj"), globe_mesh);

	GeoPos point_a{ 59.934228f, 30.324594f };
	GeoPos point_b{ 40.689167f, -74.044583f };

	Vec3f point_a_vec = geo_to_vec(point_a);
	Vec3f point_b_vec = geo_to_vec(point_b);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			gui.handleEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
		}

		clear_framebuffer(sf::Color{ 0x3e92cc }, framebuffer);
		clear_z_buffer(1.0f, z_buffer);

		draw_globe(globe_mesh, light, framebuffer, z_buffer, transforms);

		tgui::Label::Ptr azimuth_label = gui.get<tgui::Label>("azimuth_label");
		if (azimuth_label)
			azimuth_label->setText(std::format("Azimuth:\n{}", 0.0f));

		tgui::Label::Ptr elevation_label = gui.get<tgui::Label>("elevation_label");
		if (elevation_label)
			elevation_label->setText(std::format("Elevation:\n{}", 0.0f));

		texture.update(framebuffer.rgba_array.data());

		window.clear();
		window.draw(sprite);

		gui.draw();

		// Пересчитываем матрицу поворота, если пользователь изменил значения в UI.
		if (transforms.changed) {
			transforms.camera = transforms.translate_z * transforms.vert_rot * transforms.horiz_rot;
			transforms.changed = false;
		}

		window.display();
	}

	return 0;

	////////
	/*
	GeoPos point_a{ 59.934228f, 30.324594f };
	GeoPos point_b{ 40.689167f, -74.044583f };

	Vec3f point_a_vec = geo_to_vec(point_a);
	Vec3f point_b_vec = geo_to_vec(point_b);

	std::cout << "(" << point_a.lat_deg << ", " << point_a.lon_deg << ")\n";
	std::cout << "(" << point_b.lat_deg << ", " << point_b.lon_deg << ")\n\n";
	std::cout << "chord distance: " << calc_chord_distance_km(point_a_vec, point_b_vec) << " km\n";
	std::cout << "arc distance: " << calc_arc_distance_km(point_a_vec, point_b_vec) << " km\n";
	*/
}

void build_ui(tgui::Gui& gui, TransformMatrices& transforms) {
	tgui::VerticalLayout::Ptr layout = tgui::VerticalLayout::create();
	layout->setPosition(10.0f, 15.0f);
	layout->setSize(150, 220);
	layout->getRenderer()->setSpaceBetweenWidgets(5);

	tgui::EditBoxSlider::Ptr horiz_rot_slider = tgui::EditBoxSlider::create();
	horiz_rot_slider->setMinimum(0.0f);
	horiz_rot_slider->setMaximum(360.0f);
	horiz_rot_slider->setStep(0.1f);
	horiz_rot_slider->setValue(0.0f);
	horiz_rot_slider->setDecimalPlaces(1);
	horiz_rot_slider->setTextAlignment(tgui::HorizontalAlignment::Center);

	tgui::EditBoxSlider::Ptr vert_rot_slider = tgui::EditBoxSlider::create();
	vert_rot_slider->setMinimum(-90.0f);
	vert_rot_slider->setMaximum(90.0f);
	vert_rot_slider->setStep(0.1f);
	vert_rot_slider->setValue(0.0f);
	vert_rot_slider->setDecimalPlaces(1);
	vert_rot_slider->setTextAlignment(tgui::HorizontalAlignment::Center);

	horiz_rot_slider->onValueChange([&transforms](float horiz_angle_deg) {
		transforms.horiz_rot = Mat4f::create_rotation_y(horiz_angle_deg * deg_to_rad);
		transforms.changed = true;
		});
	vert_rot_slider->onValueChange([&transforms](float vert_angle_deg) {
		transforms.vert_rot = Mat4f::create_rotation_x(vert_angle_deg * deg_to_rad);
		transforms.changed = true;
		});

	layout->add(horiz_rot_slider);
	layout->add(vert_rot_slider);

	tgui::Label::Ptr azimuth_label = tgui::Label::create();
	azimuth_label->setWidgetName("azimuth_label");
	azimuth_label->setText(std::format("Azimuth:\n{}", 0.0f));

	tgui::Label::Ptr elevation_label = tgui::Label::create();
	elevation_label->setWidgetName("elevation_label");
	elevation_label->setText(std::format("Elevation:\n{}", 0.0f));

	layout->add(azimuth_label);
	layout->add(elevation_label);

	tgui::Panel::Ptr panel = tgui::Panel::create();
	panel->getRenderer()->setBackgroundColor(tgui::Color(255, 255, 255));
	panel->setPosition(10, 10);
	panel->setSize(layout->getSize().x + 20, layout->getSize().y + 20);

	panel->add(layout);
	gui.add(panel);
}

void init_transforms(TransformMatrices& transforms) {
	transforms.translate_z = Mat4f::create_translation(Vec3f{ 0.0f, 0.0f, -5.0f });
	transforms.camera = transforms.translate_z;
}

void load_model(std::string model_path, std::vector<Polygon>& polygons) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, model_path.c_str()))
		std::cout << "tinyobjloader: " << err << '\n';

	for (const auto& shape : shapes) {
		for (size_t i = 0; i <= shape.mesh.indices.size() - 3; i += 3) {
			const auto& index0 = shape.mesh.indices[i + 0];
			const auto& index1 = shape.mesh.indices[i + 1];
			const auto& index2 = shape.mesh.indices[i + 2];

			const Vertex v0{ Vec3f{
				attrib.vertices[3 * index0.vertex_index + 0],
				attrib.vertices[3 * index0.vertex_index + 1],
				attrib.vertices[3 * index0.vertex_index + 2]
			} };

			const Vertex v1{ Vec3f{
				attrib.vertices[3 * index1.vertex_index + 0],
				attrib.vertices[3 * index1.vertex_index + 1],
				attrib.vertices[3 * index1.vertex_index + 2]
			} };

			const Vertex v2{ Vec3f{
				attrib.vertices[3 * index2.vertex_index + 0],
				attrib.vertices[3 * index2.vertex_index + 1],
				attrib.vertices[3 * index2.vertex_index + 2]
			} };

			polygons.push_back(Polygon{ { v0, v1, v2 }, sf::Color::White });
		}
	}
}

void draw_globe(std::vector<Polygon> globe_mesh, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms) {
	rasterize_polygons_flat_shaded_ortho(globe_mesh, light, framebuffer, z_buffer, transforms.camera);
}

void rasterize_polygons_flat_shaded_ortho(const std::vector<Polygon>& polygons, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const Mat4f& transform) {
	// В нашем случае размеры окна никогда не меняются в рантайме.
	static const float fov_vert_rad = static_cast<float>(45.0f * deg_to_rad);
	static const float aspect_ratio = static_cast<float>(framebuffer.w) / framebuffer.h;
	static const Mat4f proj = Mat4f::create_perspective(fov_vert_rad, aspect_ratio, 0.1f, 10.0f);
	static const Mat4f viewport = Mat4f::create_viewport(framebuffer.w, framebuffer.h);

	for (Polygon polygon : polygons) {
		for (Vertex& vertex : polygon.vertices) {
			const Vec4f pos{ vertex.pos };
			vertex.pos = transform * pos;
		}

		// Вычисляем нормаль полигона.
		const Vertex& v0 = polygon.vertices[0];
		const Vertex& v1 = polygon.vertices[1];
		const Vertex& v2 = polygon.vertices[2];

		const Vec3f edge1 = v1.pos - v0.pos;
		const Vec3f edge2 = v2.pos - v0.pos;
		const Vec3f polygon_normal = Vec3f::cross(edge1, edge2).get_normalized();

		std::vector<Vertex> vertices_screen;
		for (const Vertex& vertex : polygon.vertices) {
			Vec4f pos{ vertex.pos };

			Vec4f pos_clip = proj * pos;
			if (pos_clip.w == 0.0f)
				std::cout << "";
			Vec4f pos_ndc = pos_clip / pos_clip.w;
			Vec4f pos_screen = viewport * pos_ndc;

			vertices_screen.push_back(Vertex{ Vec3f{pos_screen} });
		}

		Polygon polygon_screen{
			{vertices_screen[0], vertices_screen[1], vertices_screen[2]},
			polygon.albedo_color,
			polygon_normal
		};
		draw_polygon_flat_shaded(polygon_screen, light, framebuffer, z_buffer);
	}
}
