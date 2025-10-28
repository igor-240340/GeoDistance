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

struct GeoPoints {
	GeoPos a;
	GeoPos b;
};

struct PathLen {
	float arc_path_km;
	float straight_path_km;
};

void build_ui(tgui::Gui& gui, TransformMatrices& transforms, GeoPoints& geo_points);
void init_transforms(TransformMatrices& transforms);
void load_model(std::string model_path, std::vector<Polygon>& polygons);
void draw_earth(std::vector<Polygon> earth_mesh, const sf::Image& earth_texture_image, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms);
void rasterize_polygons_flat_shaded_textured_affine_ortho(const std::vector<Polygon>& polygons, const sf::Image& texture_image, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const Mat4f& transform);
void calc_and_draw_path(Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms, const GeoPoints& geo_points, PathLen& path_len);

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

	GeoPoints geo_points{
		{ -23.555771f, -46.639557f },
		{ 28.613830f, 77.208491f }
	};
	TransformMatrices transforms;
	init_transforms(transforms);
	build_ui(gui, transforms, geo_points);

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
	std::vector<Polygon> earth_mesh;
	load_model(std::format("assets/earth/earth.obj"), earth_mesh);

	// Читаем текстуру Земли.
	sf::Texture earth_texture;
	if (!earth_texture.loadFromFile("assets/earth/earth.jpg")) {
		std::cout << "sfml: earth_texture.loadFromFile() failed\n";
		return 1;
	}
	sf::Image earth_texture_image = earth_texture.copyToImage();

	PathLen path_len{};
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			gui.handleEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
		}

		clear_framebuffer(sf::Color{ 0x3e92cc }, framebuffer);
		clear_z_buffer(1.0f, z_buffer);

		draw_earth(earth_mesh, earth_texture_image, light, framebuffer, z_buffer, transforms);
		calc_and_draw_path(framebuffer, z_buffer, transforms, geo_points, path_len);

		tgui::Label::Ptr arc_path_label = gui.get<tgui::Label>("arc_path_label");
		if (arc_path_label)
			arc_path_label->setText(std::format("Arc path (km):\n{}", path_len.arc_path_km));

		tgui::Label::Ptr straight_path_label = gui.get<tgui::Label>("straight_path_label");
		if (straight_path_label)
			straight_path_label->setText(std::format("Straight path (km):\n{}", path_len.straight_path_km));

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
}

void build_ui(tgui::Gui& gui, TransformMatrices& transforms, GeoPoints& geo_points) {
	tgui::VerticalLayout::Ptr vert_layout = tgui::VerticalLayout::create();
	vert_layout->setPosition(10.0f, 15.0f);
	vert_layout->setSize(150, 250);
	vert_layout->getRenderer()->setSpaceBetweenWidgets(5);

	tgui::EditBoxSlider::Ptr horiz_rot_slider = tgui::EditBoxSlider::create();
	horiz_rot_slider->setMinimum(0.0f);
	horiz_rot_slider->setMaximum(360.0f);
	horiz_rot_slider->setStep(0.1f);
	horiz_rot_slider->setValue(0.0f);
	horiz_rot_slider->setDecimalPlaces(1);
	horiz_rot_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	horiz_rot_slider->onValueChange([&transforms](float horiz_angle_deg) {
		transforms.horiz_rot = Mat4f::create_rotation_y(horiz_angle_deg * deg_to_rad);
		transforms.changed = true;
		});
	vert_layout->add(horiz_rot_slider);

	tgui::EditBoxSlider::Ptr vert_rot_slider = tgui::EditBoxSlider::create();
	vert_rot_slider->setMinimum(-90.0f);
	vert_rot_slider->setMaximum(90.0f);
	vert_rot_slider->setStep(0.1f);
	vert_rot_slider->setValue(0.0f);
	vert_rot_slider->setDecimalPlaces(1);
	vert_rot_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	vert_rot_slider->onValueChange([&transforms](float vert_angle_deg) {
		transforms.vert_rot = Mat4f::create_rotation_x(vert_angle_deg * deg_to_rad);
		transforms.changed = true;
		});
	vert_layout->add(vert_rot_slider);

	tgui::HorizontalLayout::Ptr point_a_horiz_layout = tgui::HorizontalLayout::create();

	tgui::EditBoxSlider::Ptr point_a_lat_slider = tgui::EditBoxSlider::create();
	point_a_lat_slider->setMinimum(-90.0f);
	point_a_lat_slider->setMaximum(90.0f);
	point_a_lat_slider->setStep(0.01f);
	point_a_lat_slider->setValue(geo_points.a.lat_deg);
	point_a_lat_slider->setDecimalPlaces(2);
	point_a_lat_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	point_a_lat_slider->onValueChange([&geo_points](float lat_deg) {
		geo_points.a.lat_deg = lat_deg;
		});
	point_a_horiz_layout->add(point_a_lat_slider);

	tgui::EditBoxSlider::Ptr point_a_lon_slider = tgui::EditBoxSlider::create();
	point_a_lon_slider->setMinimum(-180.0f);
	point_a_lon_slider->setMaximum(180.0f);
	point_a_lon_slider->setStep(0.01f);
	point_a_lon_slider->setValue(geo_points.a.lon_deg);
	point_a_lon_slider->setDecimalPlaces(2);
	point_a_lon_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	point_a_lon_slider->onValueChange([&geo_points](float lon_deg) {
		geo_points.a.lon_deg = lon_deg;
		});
	point_a_horiz_layout->add(point_a_lon_slider);

	vert_layout->add(point_a_horiz_layout);

	tgui::HorizontalLayout::Ptr point_b_horiz_layout = tgui::HorizontalLayout::create();

	tgui::EditBoxSlider::Ptr point_b_lat_slider = tgui::EditBoxSlider::create();
	point_b_lat_slider->setMinimum(-90.0f);
	point_b_lat_slider->setMaximum(90.0f);
	point_b_lat_slider->setStep(0.01f);
	point_b_lat_slider->setValue(geo_points.b.lat_deg);
	point_b_lat_slider->setDecimalPlaces(2);
	point_b_lat_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	point_b_lat_slider->onValueChange([&geo_points](float lat_deg) {
		geo_points.b.lat_deg = lat_deg;
		});
	point_b_horiz_layout->add(point_b_lat_slider);

	tgui::EditBoxSlider::Ptr point_b_lon_slider = tgui::EditBoxSlider::create();
	point_b_lon_slider->setMinimum(-180.0f);
	point_b_lon_slider->setMaximum(180.0f);
	point_b_lon_slider->setStep(0.01f);
	point_b_lon_slider->setValue(geo_points.b.lon_deg);
	point_b_lon_slider->setDecimalPlaces(2);
	point_b_lon_slider->setTextAlignment(tgui::HorizontalAlignment::Center);
	point_b_lon_slider->onValueChange([&geo_points](float lon_deg) {
		geo_points.b.lon_deg = lon_deg;
		});
	point_b_horiz_layout->add(point_b_lon_slider);

	vert_layout->add(point_b_horiz_layout);

	tgui::Label::Ptr arc_path_label = tgui::Label::create();
	arc_path_label->setWidgetName("arc_path_label");
	arc_path_label->setText(std::format("Arc path (km):\n{}", 0.0f));

	tgui::Label::Ptr straight_path_label = tgui::Label::create();
	straight_path_label->setWidgetName("straight_path_label");
	straight_path_label->setText(std::format("Straight path (km):\n{}", 0.0f));

	vert_layout->add(arc_path_label);
	vert_layout->add(straight_path_label);

	tgui::Panel::Ptr panel = tgui::Panel::create();
	panel->getRenderer()->setBackgroundColor(tgui::Color(255, 255, 255));
	panel->setPosition(10, 10);
	panel->setSize(vert_layout->getSize().x + 20, vert_layout->getSize().y + 20);

	panel->add(vert_layout);
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
			}, TexCoord{
				attrib.texcoords[2 * index0.texcoord_index + 0],
				attrib.texcoords[2 * index0.texcoord_index + 1]
			} };

			const Vertex v1{ Vec3f{
				attrib.vertices[3 * index1.vertex_index + 0],
				attrib.vertices[3 * index1.vertex_index + 1],
				attrib.vertices[3 * index1.vertex_index + 2]
			}, TexCoord{
				attrib.texcoords[2 * index1.texcoord_index + 0],
				attrib.texcoords[2 * index1.texcoord_index + 1]
			} };

			const Vertex v2{ Vec3f{
				attrib.vertices[3 * index2.vertex_index + 0],
				attrib.vertices[3 * index2.vertex_index + 1],
				attrib.vertices[3 * index2.vertex_index + 2]
			}, TexCoord{
				attrib.texcoords[2 * index2.texcoord_index + 0],
				attrib.texcoords[2 * index2.texcoord_index + 1]
			} };

			polygons.push_back(Polygon{ { v0, v1, v2 }, sf::Color::White });
		}
	}
}

void draw_earth(std::vector<Polygon> earth_mesh, const sf::Image& earth_texture_image, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms) {
	rasterize_polygons_flat_shaded_textured_affine_ortho(earth_mesh, earth_texture_image, light, framebuffer, z_buffer, transforms.camera);
}

void rasterize_polygons_flat_shaded_textured_affine_ortho(const std::vector<Polygon>& polygons, const sf::Image& texture_image, const Light& light, Framebuffer& framebuffer, ZBuffer& z_buffer, const Mat4f& transform) {
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
			Vec4f pos_ndc = pos_clip / pos_clip.w;
			Vec4f pos_screen = viewport * pos_ndc;

			vertices_screen.push_back(Vertex{ Vec3f{pos_screen}, vertex.tex_coord });
		}

		Polygon polygon_screen{
			{vertices_screen[0], vertices_screen[1], vertices_screen[2]},
			polygon.albedo_color,
			polygon_normal
		};
		draw_polygon_flat_shaded_textured_affine(polygon_screen, texture_image, light, framebuffer, z_buffer);
	}
}

void calc_and_draw_path(Framebuffer& framebuffer, ZBuffer& z_buffer, const TransformMatrices& transforms, const GeoPoints& geo_points, PathLen& path_len) {
	bool point_a_zero = geo_points.a.lat_deg == 0.0f && geo_points.a.lon_deg == 0.0f;
	bool point_b_zero = geo_points.b.lat_deg == 0.0f && geo_points.b.lon_deg == 0.0f;
	if (point_a_zero && point_b_zero)
		return;

	// Calc.
	Vec3f point_a_vec_earth = geo_to_vec_earth(geo_points.a);
	Vec3f point_b_vec_earth = geo_to_vec_earth(geo_points.b);
	path_len.straight_path_km = calc_chord_distance_km(point_a_vec_earth, point_b_vec_earth);
	path_len.arc_path_km = calc_arc_distance_km(point_a_vec_earth, point_b_vec_earth);

	// Draw.
	// В нашем случае размеры окна никогда не меняются в рантайме.
	static const float fov_vert_rad = static_cast<float>(45.0f * deg_to_rad);
	static const float aspect_ratio = static_cast<float>(framebuffer.w) / framebuffer.h;
	static const Mat4f proj = Mat4f::create_perspective(fov_vert_rad, aspect_ratio, 0.1f, 10.0f);
	static const Mat4f viewport = Mat4f::create_viewport(framebuffer.w, framebuffer.h);

	//Vec3f point_a_vec_unit = geo_to_vec_unit(geo_points.a);
	//Vec3f point_b_vec_unit = geo_to_vec_unit(geo_points.b);

	Vec3f point_a_vec_unit = point_a_vec_earth.get_normalized();
	Vec3f point_b_vec_unit = point_b_vec_earth.get_normalized();

	float angle_cos = std::clamp(Vec3f::dot(point_a_vec_unit, point_b_vec_unit), -1.0f, 1.0f);
	float angle = std::acos(angle_cos);

	// "Базис окружности".
	float dot = Vec3f::dot(point_a_vec_unit, point_b_vec_unit);
	Vec3f b_proj = (point_a_vec_unit * dot);
	Vec3f i{ point_a_vec_unit.x, point_a_vec_unit.y, point_a_vec_unit.z };
	Vec3f j = (point_b_vec_unit - b_proj).get_normalized();
	Vec3f k = Vec3f::zero;
	Mat4f basis = Mat4f::create_basis(i, j, k);

	int n = 8;
	float angle_step = angle / n;
	float cur_angle = 0.0f;
	std::vector<Vec3f> path_points_screen; // Стартовая точка - в нуле.

	Vec4f point_world_ = transforms.camera * Vec3f::zero;
	Vec4f point_clip_ = proj * point_world_;
	Vec4f point_ndc_ = point_clip_ / point_clip_.w;
	Vec4f point_screen_ = viewport * point_ndc_;
	path_points_screen.push_back(point_screen_);
	for (int i = 0; i <= n; ++i) {
		Vec4f point_local{
			std::cos(cur_angle) * 1.03f,
			std::sin(cur_angle) * 1.03f,
			0.0f
		};
		Vec4f point_world = transforms.camera * basis * point_local;
		Vec4f point_clip = proj * point_world;
		Vec4f point_ndc = point_clip / point_clip.w;
		Vec4f point_screen = viewport * point_ndc;
		path_points_screen.push_back(Vec3f{ point_screen.x, point_screen.y, point_screen.z });
		cur_angle += angle_step;
	}

	for (int i = 0; i < path_points_screen.size(); ++i) {
		Vec3f a = path_points_screen[i];
		Vec3f b = path_points_screen[(i + 1) % path_points_screen.size()]; // Замыкаем путь на нулевую точку.
		draw_line_dda_z(a.x, a.y, a.z, b.x, b.y, b.z, sf::Color::Magenta, framebuffer, z_buffer);
	}
}
