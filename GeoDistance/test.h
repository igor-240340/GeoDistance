#pragma once

#include <iostream>
#include <cassert>

#include "geopos.h"

constexpr float epsilon = 1e-2f;

void test_0();
void test_1();
void test_2();
void test_3();
void test_4();
void test_5();
void test_6();
void test_7();
void test_8();
void test_9();
void test_10();
void test_11();
void test_12();
void test_13();
void test_14();
void run_test();

void test_0() {
	std::cout << "test_0: ";
	GeoPos point_a{ 0.0f, 0.0f };
	GeoPos point_b{ 0.0f, 0.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 0.0f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 0.0f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);
	std::cout << "PASSED\n";
}

void test_1() {
	std::cout << "test_1: ";

	GeoPos point_a{ 0.0f, 0.0f };
	GeoPos point_b{ 0.0f, 90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_2() {
	std::cout << "test_2: ";

	GeoPos point_a{ 0.0f, 0.0f };
	GeoPos point_b{ 0.0f, 180.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 12742.0f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 20015.086796020572722f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_3() {
	std::cout << "test_3: ";

	GeoPos point_a{ 0.0f, 0.0f };
	GeoPos point_b{ 0.0f, -180.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 12742.0f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 20015.086796020572722f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_4() {
	std::cout << "test_4: ";

	GeoPos point_a{ 0.0f, 0.0f };
	GeoPos point_b{ 0.0f, -90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_5() {
	std::cout << "test_5: ";

	GeoPos point_a{ 90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 0.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_6() {
	std::cout << "test_6: ";

	GeoPos point_a{ 90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_7() {
	std::cout << "test_7: ";

	GeoPos point_a{ 90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 180.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_8() {
	std::cout << "test_8: ";

	GeoPos point_a{ 90.0f, 0.0f };
	GeoPos point_b{ 0.0f, -90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_9() {
	std::cout << "test_9: ";

	GeoPos point_a{ -90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 0.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_10() {
	std::cout << "test_10: ";

	GeoPos point_a{ -90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_11() {
	std::cout << "test_11: ";

	GeoPos point_a{ -90.0f, 0.0f };
	GeoPos point_b{ 0.0f, 180.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_12() {
	std::cout << "test_12: ";

	GeoPos point_a{ -90.0f, 0.0f };
	GeoPos point_b{ 0.0f, -90.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 9009.9546058789885559f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 10007.543398010286361f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_13() {
	std::cout << "test_13: ";

	GeoPos point_a{ -90.0f, 0.0f };
	GeoPos point_b{ 90.0f, 0.0f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 12742.0f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 20015.086796020572722f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void test_14() {
	std::cout << "test_14: ";

	GeoPos point_a{ 29.91900646734f, -48.38823296416f };
	GeoPos point_b{ -19.95980752988f, 142.69801292915f };

	Vec3f point_a_vec = geo_to_vec_earth(point_a);
	Vec3f point_b_vec = geo_to_vec_earth(point_b);

	float expected_chord_distance = 12645.202480556571329685f;
	float actual_chord_distance = calc_chord_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_chord_distance - expected_chord_distance) <= epsilon);

	float expected_arc_distance = 18443.489652566398315929f;
	float actual_arc_distance = calc_arc_distance_km(point_a_vec, point_b_vec);
	assert(std::abs(actual_arc_distance - expected_arc_distance) <= epsilon);

	std::cout << "PASSED\n";
}

void run_test() {
	test_0();
	test_1();
	test_2();
	test_3();
	test_4();
	test_5();
	test_6();
	test_7();
	test_8();
	test_9();
	test_10();
	test_11();
	test_12();
	test_13();
	test_14();
	std::cout << '\n';
}