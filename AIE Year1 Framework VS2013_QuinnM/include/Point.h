#ifndef _Point_h_
#define _Point_h_


struct Point {

	Point(float in_x, float in_y) {
		x = in_x;
		y = in_y;
	}

	Point() {
		x = 0;
		y = 0;
	}

	float x, y;

	Point operator+(Point& other) {
		Point out;
		out.x = x + other.x;
		out.y = y + other.y;
		return out;
	}

	Point operator-(Point& other) {
		Point out;
		out.x = x - other.x;
		out.y = y - other.y;
		return out;
	}

	void operator+=(Point& other) {
		x += other.x;
		y += other.y;
	}

	void operator-=(Point& other) {
		x -= other.x;
		y -= other.y;
	}

	Point operator*(float other) {
		Point out;
		out.x = x * other;
		out.y = y * other;

		return out;
	}

};

#endif