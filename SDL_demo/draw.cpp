#include "draw.h"
#include "stdafx.h"

using namespace std;

void rotate(float &x, float &y, matrix &rotate_matrix)
{
	float x1 = (float)x*rotate_matrix[0][0] + (float)y*rotate_matrix[0][1];
	float y1 = (float)x*rotate_matrix[1][0] + (float)y*rotate_matrix[1][1];
	x = x1;
	y = y1;
}

void scale(float &x, float &y, float scale_x, float scale_y) {
	x *= scale_x;
	y *= scale_y;
}

void offset(float &x, float &y, float x_zero_offset, float y_zero_offset) {
	x += x_zero_offset;
	y += y_zero_offset;
}

struct Point
{
	double x;
	double y;
};

struct Line
{
	Point start;
	Point end;
	int visible; // 1 - visible, 0 - dashed, -1 - invisible
};

#define COMPLETELY_INSIDE 0
#define COMPLETELY_OUTSIDE 2
#define INTERSECTS 1

class Rect;

int RectangleClipLine(Rect * rect, double &x1, double &y1, double &x2, double &y2);


class Rect{
public:
	int x1;
	int y1;
	int x2;
	int y2;
	double angle;

	Rect(int x1, int y1, int x2, int y2, double angle = 0)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->angle = angle;
	}

	vector<Line> RectCreateLines()
	{
		vector<Line> lines;
		Line m1 = { {this->x1, this->y1},  {this->x2, this->y1}, 1 };
		Line m2 = { {this->x2, this->y1},  {this->x2, this->y2}, 1 };
		Line m3 = { {this->x2, this->y2},  {this->x1, this->y2}, 1 };
		Line m4 = { {this->x1, this->y2},  {this->x1, this->y1}, 1 };
		lines.push_back(m1);
		lines.push_back(m2);
		lines.push_back(m3);
		lines.push_back(m4);
		return lines;
	}

	// completely_invisible = 1 - если нивидимая линия должна полностью отсутсвовать
	// clip_inside = 1 - если видимая часть находится внутри
	vector<Line> RectClipLine(Line *line, int clip_inside=0, int completely_invisible=0)
	{
		vector<Line> res;

		double x1 = line->start.x, y1 = line->start.y, x2 = line->end.x, y2 = line->end.y, res1, res2;
		res1 = RectangleClipLine(this, x1, y1, x2, y2);
		//res2 = res1;
		res2 = RectangleClipLine(this, x1, y1, x2, y2);


		if (res1 == COMPLETELY_OUTSIDE || res2 == COMPLETELY_OUTSIDE)
		{
			Line l = *line;
			//l.visible = 1; видимую линию оставляем как есть
			if (clip_inside) // внутри должно быть видимое
				l.visible = 0 - completely_invisible; // значит снаружи уже невидимое
			res.push_back(l); // returning source line
			return res;
		}

		if (res1 == COMPLETELY_INSIDE)
		{
			Line l = *line;
			if (!clip_inside) // снаружи должно быть видимое
				l.visible = 0 - completely_invisible; // внутри делаем невидимое
			res.push_back(l); // returning source line
			return res;
		}

		int visible;
		if (line->start.x != x1 || line->start.y != y1) {
			visible = line->visible;
			if (clip_inside)
				visible = 0 - completely_invisible;
			Line l1 = { {line->start.x, line->start.y}, {x1, y1}, visible };
			res.push_back(l1);
		}
		visible = line->visible;
		if (!clip_inside)
			visible = 0 - completely_invisible;
		Line l2 = { {x1, y1}, {x2, y2}, visible };
		res.push_back(l2);
		if (line->end.x != x2 || line->end.y != y2) {
			visible = line->visible;
			if (clip_inside)
				visible = 0 - completely_invisible;
			Line l1 = { {x2, y2}, {line->end.x, line->end.y}, visible };
			res.push_back(l1);
		}

		return res;
	}

	Point Center()
	{
		Point c = { (x2 + x1) / 2, (y2 + y1) / 2 };
		return c;
	}
};



Rect clippingRect = { 150, 150, 400, 300 };
//Rect clippingRect1 = { 200, 100, 600, 200 };

int RectangleClipPoint(Rect * rect, double x, double y)
{
	int res = 0;
	double x1, x2, y1, y2;

	if (x < rect->x1)
		res |= 0x8; // 1000
	else if (rect->x2 < x)
		res |= 0x4; // 0100
	if (y < rect->y1)
		res |= 0x1; // 0001
	else if (rect->y2 < y)
		res |= 0x2; // 0010

	return res;
}

int BorderClipPoint(Rect * rect, double x, double y)
{
	int res = 0;
	int x1, x2, y1, y2;

	if (x <= rect->x1)
		res |= 0x8; // 1000
	else if (rect->x2 <= x)
		res |= 0x4; // 0100
	if (y <= rect->y1)
		res |= 0x1; // 0001
	else if (rect->y2 <= y)
		res |= 0x2; // 0010

	return res;
}



int RectangleClipLine(Rect * rect, double &x1, double &y1, double &x2, double &y2)
{
	/*
	1001 0001 0101
	1000 0000 0100
	1010 0010 0110
	*/
	int start, end, start_b, end_b;
	start = RectangleClipPoint(rect, x1, y1);
	end = RectangleClipPoint(rect, x2, y2);

	start_b = BorderClipPoint(rect, x1, y1);
	end_b = BorderClipPoint(rect, x2, y2);
	//printf("Start: %d End: %d", start_b, end_b);

	if (!(start & start_b) && !(end & end_b)) // both points in center, check border too
		return COMPLETELY_INSIDE;

	if (start_b & end_b) // check including borders
		return COMPLETELY_OUTSIDE;

	double A = (y1 - y2), B = (x2 - x1), C = (x1*y2 - x2 * y1);

	// check excluding borders
	// START
	if (start & 0x8) // 1000
	{
		x1 = rect->x1;
		y1 = -(A*x1 + C) / B;
	}
	if (start & 0x4) // 0100
	{
		x1 = rect->x2;
		y1 = -(A*x1 + C) / B;
	}
	if (start & 0x1) // 0001
	{
		y1 = rect->y1;
		x1 = -(B*y1 + C) / A;
	}
	if (start & 0x2) // 0010
	{
		y1 = rect->y2;
		x1 = -(B*y1 + C) / A;
	}

	// END
	if (end & 0x8) // 1000
	{
		x2 = rect->x1;
		y2 = -(A*x2 + C) / B;
	}
	if (end & 0x4) // 0100
	{
		x2 = rect->x2;
		y2 = -(A*x2 + C) / B;
	}
	if (end & 0x1) // 0001
	{
		y2 = rect->y1;
		x2 = -(B*y2 + C) / A;
	}
	if (end & 0x2) // 0010
	{
		y2 = rect->y2;
		x2 = -(B*y2 + C) / A;
	}

	//printf("(%d %d) (%d %d)", x1, y1, x2, y2);

	return INTERSECTS;
}

void My_DrawDashedLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, int dash_len) {
	const int deltaX = abs(x2 - x1);
	const int deltaY = abs(y2 - y1);
	const int signX = x1 < x2 ? 1 : -1;
	const int signY = y1 < y2 ? 1 : -1;
	//
	int error = deltaX - deltaY;
	int count = 0;
	int draw = 1;
	//
	
	SDL_RenderDrawPoint(renderer, x2, y2);
	while (x1 != x2 || y1 != y2)
	{
		if (draw)
			SDL_RenderDrawPoint(renderer, x1, y1);

		const int error2 = error * 2;
		//
		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}
		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
		count++;
		if (count == dash_len)
		{
			count = 0;
			draw = !draw;
		}
	}
}

#define STYLE_DRAW_ONLY_INSIDE 0
#define STYLE_DRAW_DASHED_INSIDE 1
#define STYLE_DRAW_DASHED_OUTSIDE 2

vector<Line> RectClipLine(Rect *cr, Line *line)
{
	vector<Line> res;

	double x1 = line->start.x, y1 = line->start.y, x2 = line->end.x, y2 = line->end.y, res1, res2;
	res1 = RectangleClipLine(cr, x1, y1, x2, y2);
	//res2 = res1;
	res2 = RectangleClipLine(cr, x1, y1, x2, y2);


	if (res1 == COMPLETELY_OUTSIDE || res2 == COMPLETELY_OUTSIDE)
	{
		Line l = *line;
		l.visible = 1;
		res.push_back(l); // returning source line
		return res;
	}

	if (res1 == COMPLETELY_INSIDE)
	{
		Line l = *line;
		l.visible = 0;
		res.push_back(l); // returning source line
		return res;
	}

	if (line->start.x != x1 || line->start.y != y1) {
		Line l1 = { {line->start.x, line->start.y}, {x1, y1}, 1 };
		res.push_back(l1);
	}
	Line l2 = { {x1, y1}, {x2, y2}, 0 };
	res.push_back(l2);
	if (line->end.x != x2 || line->end.y != y2) {
		Line l1 = { {x2, y2}, {line->end.x, line->end.y}, 1 };
		res.push_back(l1);
	}

	return res;
}

/*void DrawClippedLine(SDL_Renderer *renderer, Rect * cr, int x_1, int y_1, int x_2, int y_2, int style, int dash = 5)
{
	Line original = { {x_1, y_1}, {x_2, y_2}, 0 };
	vector<Line> lines = RectClipLine(cr, &original);

	for (auto l : lines)
	{
		if (l.visible == 1)
		{
			if (style == STYLE_DRAW_DASHED_OUTSIDE)
				My_DrawDashedLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y, dash);
			if (style == STYLE_DRAW_DASHED_INSIDE)
				SDL_RenderDrawLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y);
		}
		if (l.visible == 0)
		{
			if (style == STYLE_DRAW_DASHED_INSIDE)
				My_DrawDashedLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y, dash);
			if (style == STYLE_DRAW_DASHED_OUTSIDE || style == STYLE_DRAW_ONLY_INSIDE)
				SDL_RenderDrawLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y);
		}
	}
}*/

// нарисовать линию по отсекающему прямоугольнику
void DrawClippedLine(SDL_Renderer *renderer, Rect * cr, Line * line, int dash = 5)
{
	vector<Line> lines = cr->RectClipLine(line, 1, 1); // Completely invisible

	for (auto l : lines)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
		if (l.visible == 1)
		{
			SDL_RenderDrawLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y);
		}
		if (l.visible == 0)
		{
			My_DrawDashedLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y, dash);
		}
		if (l.visible == -1)
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
			My_DrawDashedLine(renderer, l.start.x, l.start.y, l.end.x, l.end.y, dash);
		}
	}
}


vector<Line> CreateScene(Rect * masterRect, Rect * slaveRect)
{
	vector<Line> scene;
	vector<Line> originalSlaveRect;

	// наша сцена - множество линий (видимых, невидимых-пунктирных)
	scene = masterRect->RectCreateLines(); // начинаем создание сцены с главного прямоугольника

	originalSlaveRect = slaveRect->RectCreateLines(); // 4 отрезка второго прямоугольника. Их будем разрезать первым прямоугольником


	matrix point = matrix_init(3, 1);
	matrix res_point = matrix_init(3, 1);
	matrix transform_matrix = matrix_init(3, 3); // инициализируем единичной матрицей

	Point master_center = masterRect->Center();

	// Вращение дочернего относительно центра основного прямоугольника в обратную сторону
	matrix slaveRotatorMaster = rotation(-masterRect->angle);
	matrix move_center_ms = translation(master_center.x, master_center.y); // смещение в (0, 0)
	matrix move_back_ms = translation(-master_center.x, -master_center.y); // смещение в исходную позицию

	transform_matrix = matrix_multiply(transform_matrix, move_back_ms);
	transform_matrix = matrix_multiply(transform_matrix, slaveRotatorMaster); // R - вращение: X = T * R
	transform_matrix = matrix_multiply(transform_matrix, move_center_ms);

	// Вращение второго прямоугольника относительно своего центра
	matrix slaveRotator = rotation(slaveRect->angle);
	Point slave_center = slaveRect->Center();

	matrix move_center = translation(slave_center.x, slave_center.y); // смещение в (0, 0)
	matrix move_back = translation(-slave_center.x, -slave_center.y); // смещение в исходную позицию

	transform_matrix = matrix_multiply(transform_matrix, move_back);
	transform_matrix = matrix_multiply(transform_matrix, slaveRotator); // R - вращение: X = T * R
	transform_matrix = matrix_multiply(transform_matrix, move_center);
	//transform_matrix = matrix_multiply(transform_matrix, move_back);

	for (auto line : originalSlaveRect)
	{
		point[0][0] = line.start.x;
		point[1][0] = line.start.y;
		point[2][0] = 1;
		matrix_multiply(transform_matrix, point, res_point);
		line.start.x = res_point[0][0];
		line.start.y = res_point[1][0];
		point[0][0] = line.end.x;
		point[1][0] = line.end.y;
		point[2][0] = 1;
		matrix_multiply(transform_matrix, point, res_point);
		line.end.x = res_point[0][0];
		line.end.y = res_point[1][0];
		

		auto derivedLines = masterRect->RectClipLine(&line);
		for (auto newLine : derivedLines)
			scene.push_back(newLine);
	}

	// вращение основного прямоугольника (вместе со вторым) - т.е. всей сцены
	matrix masterRotator = rotation(masterRect->angle);
	matrix move_center_master = translation(master_center.x, master_center.y); // смещение в (0, 0)
	matrix move_back_master = translation(-master_center.x, -master_center.y); // смещение в исходную позицию

	transform_matrix = matrix_init(3, 3); // переинициализируем матрицу преобразования

	transform_matrix = matrix_multiply(transform_matrix, move_back_ms);
	transform_matrix = matrix_multiply(transform_matrix, masterRotator); // R - вращение: X = T * R
	transform_matrix = matrix_multiply(transform_matrix, move_center_ms);
	for (auto &line : scene)
	{
		point[0][0] = line.start.x;
		point[1][0] = line.start.y;
		point[2][0] = 1;
		matrix_multiply(transform_matrix, point, res_point);
		line.start.x = res_point[0][0];
		line.start.y = res_point[1][0];
		point[0][0] = line.end.x;
		point[1][0] = line.end.y;
		point[2][0] = 1;
		matrix_multiply(transform_matrix, point, res_point);
		line.end.x = res_point[0][0];
		line.end.y = res_point[1][0];
	}

	return scene;
}

void DrawScene(SDL_Renderer *renderer, double angle_master, double angle_slave, int size_x, int size_y, int master_x1, int master_y1, int slave_x1, int slave_y1)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
	//SDL_RenderDrawLine(renderer, 10, 10, 100+scale, 100);
	//SDL_Rect r = { clippingRect.x1, clippingRect.y1, clippingRect.x2 - clippingRect.x1, clippingRect.y2 - clippingRect.y1 };
	
	Rect clippingRect((SCREEN_WIDTH - size_x) / 2, (SCREEN_HEIGHT - size_y) / 2, (SCREEN_WIDTH + size_x) / 2, (SCREEN_HEIGHT + size_y) / 2);
	
	SDL_Rect A = {clippingRect.x1, clippingRect.y1, size_x, size_y};
	SDL_RenderDrawRect(renderer, &A);

	Rect master( master_x1, master_y1, master_x1 + 250, master_y1 + 150, angle_master * M_PI / 180);
	Rect slave(slave_x1, slave_y1, slave_x1 + 400, slave_y1 + 100, -(angle_slave/2) * M_PI / 180);

	vector<Line> lines;
	lines = CreateScene(&master, &slave);

	for (auto line : lines)
	{
		DrawClippedLine(renderer, &clippingRect, &line, 5);
		//if(line.visible)
		//	SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
		//else
		//	My_DrawDashedLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y, 5);
	}
	
	//SDL_Rect r1 = { clippingRect1.x1, clippingRect1.y1, clippingRect1.x2 - clippingRect1.x1, clippingRect1.y2 - clippingRect1.y1 };

	//SDL_RenderDrawRect(renderer, &r);
	
	
	
	//DrawRect(renderer, &clippingRect1);
	//SDL_RenderDrawRect(renderer, &r1);

	//DrawClippedLine(renderer, &clippingRect, 50, 50, 300, 450, STYLE_DRAW_DASHED_INSIDE);
	//DrawClippedLine(renderer, &clippingRect, 80, 50, 600, 430, STYLE_DRAW_DASHED_INSIDE);

	// completely outside {150, 150, 400, 300};
	//DrawClippedLine(renderer, &clippingRect, 50, 250, 300, 450, STYLE_DRAW_DASHED_INSIDE);
	//DrawClippedLine(renderer, &clippingRect, 350, 460, 630, 200, STYLE_DRAW_DASHED_INSIDE);

	//DrawClippedLine(renderer, &clippingRect, 320, 410, 500, 50, STYLE_DRAW_DASHED_INSIDE);

	//DrawClippedLine(renderer, &clippingRect, 50, 300, 150, 450, STYLE_DRAW_DASHED_OUTSIDE);

	/*My_DrawDashedLine(renderer, x1, y1, x2, y2, 5);
	res = RectangleClipLine(&clippingRect, x1, y1, x2, y2);
	if (res == COMPLETELY_OUTSIDE)
		return;
	res = RectangleClipLine(&clippingRect, x1, y1, x2, y2);
	if (res == COMPLETELY_OUTSIDE)
		return;
	//printf("%d", rv);

	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);*/

	//My_DrawDashedLine(renderer, 30, 10, 30, 100, 5);
	//My_DrawDashedLine(renderer, 30, 100, 200, 150, 5);
	//My_DrawDashedLine(renderer, 200, 150, 300, 50, 5);
}