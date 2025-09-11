#pragma once

#include <numbers>
#include <algorithm>

#include "Vec3f.h"

static constexpr float mean_earth_r_km = 6371.0f;

constexpr float deg_to_rad = static_cast<float>(std::numbers::pi / 180.0);
constexpr float rad_to_deg = 1.0f / deg_to_rad;

struct GeoPos {
	float lat_deg;
	float lon_deg;
};

Vec3f geo_to_vec(const GeoPos& point_geo) {
	float lat_rad = point_geo.lat_deg * deg_to_rad;
	float lon_rad = point_geo.lon_deg * deg_to_rad;

	float equator_proj = std::cos(lat_rad);
	Vec3f unit_vec{
		std::sin(lon_rad) * equator_proj,
		std::sin(lat_rad),
		std::cos(lon_rad) * equator_proj
	};

	return unit_vec * mean_earth_r_km;
}

// По сути - это длина дуги на окружности, образованной этими векторами.
float calc_arc_distance_km(const Vec3f& a, const Vec3f& b) {
	float dot = Vec3f::dot(a, b);
	float angle_cos = std::clamp(dot / (a.length() * b.length()), -1.0f, 1.0f);
	float angle = std::acos(angle_cos);
	return angle * mean_earth_r_km;
}

float calc_chord_distance_km(const Vec3f& a, const Vec3f& b) {
	return (a - b).length();
}
