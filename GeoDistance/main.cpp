#include <iostream>

#include "geopos.h"

#ifdef _DEBUG
#include "test.h"
#endif // _DEBUG

int main() {
#ifdef _DEBUG
	run_all_tests();
#endif // _DEBUG

	GeoPos point_a{ 39.76424445f, -104.8547524f };
	GeoPos point_b{ 56.02672945f, 92.86573395f };

	//GeoPos point_a{ -23.555771f, -46.639557f };
	//GeoPos point_b{ 28.613830f, 77.208491f };

	Vec3f point_a_vec = geo_to_vec(point_a);
	Vec3f point_b_vec = geo_to_vec(point_b);

	std::cout << "(" << point_a.lat_deg << ", " << point_a.lon_deg << ")\n";
	std::cout << "(" << point_b.lat_deg << ", " << point_b.lon_deg << ")\n\n";
	std::cout << "chord distance: " << calc_chord_distance_km(point_a_vec, point_b_vec) << " km\n";
	std::cout << "arc distance: " << calc_arc_distance_km(point_a_vec, point_b_vec) << " km\n";

	return 0;
}
