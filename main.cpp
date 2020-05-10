#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string> 
#include <cstdlib>
#include <list>
#include "windows.h"


#define CharacterSize 12
#define RectangleOutline 2
#define WindowWidth 1026
#define WindowHeight 770
#define RectangleWidth 64
#define RectangleHeight 32
#define MaxMapSizeX 20
#define MaxMapSizeY 20
#define CharacterSizeNumber 9

struct WallAround
{
	bool up;
	bool down;
	bool left;
	bool right;

	WallAround()
	{
		up = true;
		down = true;
		left = true;
		right = true;
	}

	void print()
	{
		std::cout << "Wall up: " << up << std::endl << "Wall down: " << down << std::endl;
		std::cout << "Wall left: " << left << std::endl << "Wall right: " << right<< std::endl;
	}
};

struct Point
{
public:
	int x;
	int y;

	Point()
	{
		x = -1;
		y = -1;
	}
};

//structure for save current map size
struct MapSize
{
public:
	int width;
	int height;

	MapSize()
	{
		width = 0;
		height = 0;
	}
};

//structure to return from load func
struct Map
{
public:
	MapSize map_size;
	char** array;
};

struct RGBColor
{
public:
	int red;
	int green;
	int blue;
	RGBColor()
	{
		red = 0;
		green = 0;
		blue = 0;
	}
	RGBColor(int r, int g, int b)
	{
		red = r;
		green = g;
		blue = b;
	}
	void print()
	{
		std::cout << "R: " << red << " G: " << green << " B: " << blue;
	}
};
//platform is block in the map whitch can be road or wall
class Platform
{
public:
	int x_in_grid, y_in_grid;
	int x_left_up_rect, y_left_up_rect;
	int size_x, size_y;
	int value;
	bool isWall;
	bool isStartPoint;
	bool isFinishPoint;
	bool isWay;
	RGBColor color;
	

	Platform()
	{
		x_in_grid = 0;
		y_in_grid = 0;
		x_left_up_rect = 0;
		y_left_up_rect = 0;
		size_x = 0;
		size_y = 0;
		value = 0;
		isWall = false;
		isStartPoint = false;
		isFinishPoint = false;
		isWay = false;
		color = RGBColor();
	}

	Platform(int X_in_grid, int Y_in_grid, int X_left_up_rect, int Y_left_up_rect, int Size_x, int Size_y)
	{
		x_in_grid = X_in_grid;
		y_in_grid = Y_in_grid;
		x_left_up_rect = X_left_up_rect;
		y_left_up_rect = Y_left_up_rect;
		size_x = Size_x;
		size_y = Size_y;
		value = 0;
		isWall = false;
		isStartPoint = false;
		isFinishPoint = false;
		isWay = false;
		color = RGBColor();
	}

	//draw platform

	void draw(sf::RenderWindow& window, sf::Color main_color, sf::Color color_outline)
	{
		sf::RectangleShape rectangle(sf::Vector2f(size_x- RectangleOutline, size_y- RectangleOutline));

		if (not isWall)
		{
			//white objects are road
			rectangle.setFillColor(main_color);
		}
		else 
			//black objects are wall
			rectangle.setFillColor(sf::Color::Black);


		if (isStartPoint || isFinishPoint)
		{
			rectangle.setFillColor(sf::Color(0,0,205));
		}


		rectangle.setOutlineThickness(RectangleOutline);
		rectangle.setOutlineColor(color_outline);
		rectangle.move(x_left_up_rect + RectangleOutline,y_left_up_rect + RectangleOutline);
		
		window.draw(rectangle);

		sf::Font mainFont;
		mainFont.loadFromFile("arial.ttf");

		//start point is A
		if (isStartPoint)
		{
			sf::Text text;
			text.setFont(mainFont);
			text.setString("A");
			text.setFillColor(sf::Color::Red);
			text.setCharacterSize(CharacterSize);
			text.setPosition(x_left_up_rect + (size_x - CharacterSize)/2 , y_left_up_rect + (size_y  - CharacterSize)/2);
			text.setStyle(sf::Text::Bold);
			window.draw(text);
		}
		//finish point is B
		else if (isFinishPoint)
		{
			sf::Text text;
			text.setFont(mainFont);
			text.setString("B");
			text.setFillColor(sf::Color::Red);
			text.setCharacterSize(CharacterSize);
			text.setPosition(x_left_up_rect + (size_x - CharacterSize) / 2, y_left_up_rect + (size_y - CharacterSize) / 2);
			text.setStyle(sf::Text::Bold);
			window.draw(text);
		}

		
	}

	//some setters for platform
	void set_wall()
	{
		isWall = true;
	}

	void set_start_point()
	{
		isStartPoint = true;
		value = 0;
		color = RGBColor(255, 255, 51);
	}

	void set_finish_point()
	{
		isFinishPoint = true;
		color = RGBColor(255, 153, 51);
	}

	void set_value(int val)
	{
		value = val;
	}

	void set_color(int r, int g, int b)
	{
		color = RGBColor(r, g, b);
	}

	void set_to_way()
	{
		isWay = true;
	}

	void print_debug_info()
	{
		std::cout << "x in grid: " << x_in_grid << " y in grid: " << y_in_grid << " color: ";
		color.print();
		std::cout << std::endl<<" value: " << value << "is wall " <<isWall<< " is way "<< isWay<<std::endl;
	}
};


//load map from .txt and convert it in char*
Map load_map()
{
	std::ifstream fin("map.txt");
	if (!fin.is_open())
		std::cout << "Can't open file with map!" << std::endl;
	else
	{
		std::string size;

		MapSize map_size;

		fin >> map_size.height >> map_size.width;
		if (map_size.width > MaxMapSizeX || map_size.height > MaxMapSizeY || map_size.width <= 0 || map_size.height <= 0)
		{
			std::cout << "Bad size of map!" << std::endl;
		}
		std::cout << "Map size: ";
		std::cout << map_size.height << "x" << map_size.width << std::endl << std::endl;
		char** data = new char* [map_size.height];
		std::cout << "Map: " << std::endl << std::endl;
		for (int i = 0; i < map_size.height; i++)
		{
			data[i] = new char[map_size.width];
			for (int j = 0; j < map_size.width; j++)
			{
				fin >> data[i][j];
				std::cout << data[i][j]<<" ";
			}
			std::cout << std::endl;
		}
		Map param;
		param.map_size = map_size;
		param.array = data;
		return param;
	}

}

void resize_to_min(sf::RenderWindow &window)
{
	if (window.getSize().x < sf::Vector2u(WindowWidth, WindowHeight).x)
		window.setSize(sf::Vector2u(WindowWidth, window.getSize().y));
	if (window.getSize().y < sf::Vector2u(WindowWidth, WindowHeight).y)
		window.setSize(sf::Vector2u(window.getSize().x, WindowHeight));
}


void draw_value(sf::RenderWindow& window,Platform rectangle)
{
	if (rectangle.value > 0)
	{
		sf::Font mainFont;
		mainFont.loadFromFile("arial.ttf");
		sf::Text text;
		text.setFont(mainFont);
		text.setString(std::to_string(rectangle.value));
		text.setFillColor(sf::Color::White);
		text.setCharacterSize(CharacterSizeNumber);
		text.setPosition(rectangle.x_left_up_rect + (rectangle.size_x) / 10, rectangle.y_left_up_rect + (rectangle.size_y) / 10);
		text.setStyle(sf::Text::Bold);
		window.draw(text);
	}

}


void draw_way(Platform** platforms, MapSize map, sf::RenderWindow& window)
{
	for (int i = 0; i < map.height; i++)
	{
		for (int j = 0; j < map.width; j++)
		{
			if (platforms[i][j].isWay)
			{
				platforms[i][j].draw(window, sf::Color(platforms[i][j].color.red, platforms[i][j].color.green, platforms[i][j].color.blue), sf::Color::Red);
				draw_value(window, platforms[i][j]);
			}
		}
	}
}

std::vector<std::list<Platform>> platform_sort(Platform** platforms,MapSize map)
{
	std::vector<Platform> tmp_array;
	int k = 0;

	for (int i = 0; i < map.height; i++)
	{
		for (int j = 0; j < map.width; j++)
		{
			tmp_array.push_back(platforms[i][j]);
			k++;
		}
	}

	std::sort(tmp_array.begin(), tmp_array.end(), [](const Platform& a, const Platform& b) {return a.value < b.value;});

	std::vector<std::list<Platform>> vector_of_lists_of_platforms;

	int max = tmp_array.back().value;

	for (int i = 0; i < (max + 1); i++)
	{
		std::list<Platform> list;
		vector_of_lists_of_platforms.push_back(list);
	}

	for (int i = 0; i < k; i++)
	{
		vector_of_lists_of_platforms[tmp_array[i].value].push_back(tmp_array[i]);
	}
	/*
	int old_i = 0;
	std::list<Platform> list;
	for (int i = 0; i < k; i++)
	{
		if (tmp_array[i].value == old_i)
		{
			list.push_back(tmp_array[i]);
		}
		else
		{
			vector_of_lists_of_platforms.push_back(list);
			list.clear();
			old_i++;
		}

	}
	*/
	/*
	int k = 0;
	Platform* tmp_array = new Platform[map.height * map.width];
	for (int i = 0; i < map.height; i++)
	{
		for (int j = 0; j < map.width; j++)
		{
			tmp_array[k] = platforms[i][j];
			k++;
		}
	}

	for (int i = 0; i < k - 1; i++)
	{
		for (int j = i+1; j < k; j++)
		{
			if (tmp_array[i].value > tmp_array[j].value)
			{
				Platform tmp;
				tmp = tmp_array[i];
				tmp_array[i] = tmp_array[j];
				tmp_array[j] = tmp;
			}
		}
	}
	std::vector<std::list<Platform>> vector_of_lists_of_platforms;
	int old_i = 0;
	std::list<Platform> list;
	for (int i=0;i<k;i++)
	{
		if (tmp_array[i].value == old_i)
		{
				list.push_back(tmp_array[i]);
		}
		else
		{
			vector_of_lists_of_platforms.push_back(list);
			list.clear();
			old_i++;
		}
			
	}
	*/
	return vector_of_lists_of_platforms;
}

Platform** step_of_wave_search(Platform** platforms, int x_start, int y_start, int x_finish, int y_finish,sf::RenderWindow& window)
{

	WallAround wall = WallAround();
	int i = 0;
	int j = 1;
	std::list <Platform> current;
	std::list <Platform> next_step;
	
	int new_size = 0;
	bool isWork = true;;

	int red_num = 10;
	int green_num = 40;
	int blue_num = 10;
	current.push_back(platforms[x_start][y_start]);
	int old_value = 0;
	while (true)
	{
		if (current.back().x_in_grid == x_finish && current.back().y_in_grid == y_finish)
		{
			platforms[current.back().x_in_grid][current.back().y_in_grid].set_color(255,153,51);
			break;
		}
		int old_j = j;
		if (!platforms[current.back().x_in_grid + 1][current.back().y_in_grid].isWall && platforms[current.back().x_in_grid + 1][current.back().y_in_grid].value == 0 && !platforms[current.back().x_in_grid + 1][current.back().y_in_grid].isStartPoint)
		{
			platforms[current.back().x_in_grid + 1][current.back().y_in_grid].set_value(current.back().value+1);
			platforms[current.back().x_in_grid + 1][current.back().y_in_grid].set_color(red_num, green_num, blue_num);
			//platforms[current.back().x_in_grid + 1][current.back().y_in_grid].print_debug_info();
			next_step.push_back(platforms[current.back().x_in_grid + 1][current.back().y_in_grid]);
			if (current.back().x_in_grid + 1 == x_finish && current.back().y_in_grid == y_finish)
				break;
			j++;

		}
		if (!platforms[current.back().x_in_grid][current.back().y_in_grid + 1].isWall && platforms[current.back().x_in_grid][current.back().y_in_grid + 1].value == 0 && !platforms[current.back().x_in_grid][current.back().y_in_grid+1].isStartPoint)
		{
			platforms[current.back().x_in_grid][current.back().y_in_grid + 1].set_value(current.back().value + 1);
			platforms[current.back().x_in_grid][current.back().y_in_grid + 1].set_color(red_num, green_num, blue_num);
			//platforms[current.back().x_in_grid][current.back().y_in_grid + 1].print_debug_info();
			next_step.push_back(platforms[current.back().x_in_grid][current.back().y_in_grid + 1]);
			if (current.back().x_in_grid  == x_finish && current.back().y_in_grid + 1== y_finish)
				break;
			j++;
		}
		if (!platforms[current.back().x_in_grid - 1][current.back().y_in_grid].isWall && platforms[current.back().x_in_grid - 1][current.back().y_in_grid].value == 0 && !platforms[current.back().x_in_grid - 1][current.back().y_in_grid].isStartPoint)
		{
			platforms[current.back().x_in_grid - 1][current.back().y_in_grid].set_value(current.back().value + 1);
			platforms[current.back().x_in_grid - 1][current.back().y_in_grid].set_color(red_num, green_num, blue_num);
			//platforms[current.back().x_in_grid - 1][current.back().y_in_grid].print_debug_info();
			next_step.push_back(platforms[current.back().x_in_grid - 1][current.back().y_in_grid]);
			if (current.back().x_in_grid - 1 == x_finish && current.back().y_in_grid == y_finish)
				break;
			j++;
		}
		if (!platforms[current.back().x_in_grid][current.back().y_in_grid - 1].isWall && platforms[current.back().x_in_grid][current.back().y_in_grid - 1].value == 0 && !platforms[current.back().x_in_grid][current.back().y_in_grid-1].isStartPoint)
		{
			platforms[current.back().x_in_grid][current.back().y_in_grid - 1].set_value(current.back().value + 1);
			platforms[current.back().x_in_grid][current.back().y_in_grid - 1].set_color(red_num, green_num, blue_num);
			//platforms[current.back().x_in_grid][current.back().y_in_grid - 1].print_debug_info();
			next_step.push_back(platforms[current.back().x_in_grid][current.back().y_in_grid - 1]);
			if (current.back().x_in_grid == x_finish && current.back().y_in_grid - 1 == y_finish)
				break;
			j++;
		}
		if (old_j != j && old_value== current.back().value)
		{
			red_num += 1;
			green_num += 2.5;
			blue_num += 1.5;
		}
		if (next_step.size() <= 0)
			break;

		
		
		//break;
		old_value = current.back().value;
		current.push_back(next_step.front());
		next_step.pop_front();
	}
	return platforms;
}

Platform** find_way(Platform** platforms, int x_finish, int y_finish, sf::RenderWindow& window)
{
	int i = x_finish;
	int j = y_finish;
	bool isFinished = true;
	Platform current = platforms[i][j];
	platforms[i][j].set_to_way();
	while (current.value != 0)
	{
		isFinished = true;
		if (platforms[i+1][j].value < current.value && !platforms[i+1][j].isWall)
		{
			isFinished = false;
			platforms[i + 1][j].set_to_way();
			current = platforms[i+1][j];
			i++;
		}
		else if (platforms[i][j+1].value < current.value && !platforms[i][j+1].isWall)
		{
			isFinished = false;
			platforms[i][j + 1].set_to_way();
			current = platforms[i][j+1];
			j++;
		} else if (platforms[i-1][j].value < current.value && !platforms[i-1][j].isWall)
		{
			isFinished = false;
			platforms[i - 1][j].set_to_way();
			current = platforms[i-1][j];
			i--;
		} else if (platforms[i][j-1].value < current.value && !platforms[i][j-1].isWall)
		{
			isFinished = false;
			platforms[i][j - 1].set_to_way();
			current = platforms[i][j-1];
			j--;
		}
		if (isFinished == true)
		{
			break;
		}

	}
	return platforms;
}

//block of code for graphics
void draw_graphics(Map map)
{
	sf::RenderWindow window(sf::VideoMode(map.map_size.width*RectangleWidth+RectangleOutline, map.map_size.height*RectangleHeight+RectangleOutline), "Wave_alg", sf::Style::Default);
	resize_to_min(window);
		window.setSize(sf::Vector2u(window.getSize().x, 770));


	Platform** platforms = new Platform * [map.map_size.height];
	Point start_point, finish_point;

	int iteration = 0;

	for (int i = 0; i < map.map_size.height; i++)
	{
		platforms[i] = new Platform[map.map_size.width];
		for (int j = 0; j < map.map_size.width; j++)
		{
			platforms[i][j] = Platform(i, j, RectangleWidth * j, RectangleHeight * i, RectangleWidth, RectangleHeight);
			if (map.array[i][j] == '1')
				platforms[i][j].set_wall();
			else if (map.array[i][j] == 'A')
			{
				platforms[i][j].set_start_point();
				start_point.x = i;
				start_point.y = j;
				//std::cout << i << " " << j << std::endl;
			}
			else if (map.array[i][j] == 'B')
			{
				platforms[i][j].set_finish_point();
				finish_point.x = i;
				finish_point.y = j;
			}
		}
	}

	for (int i = 0; i < map.map_size.height; i++)
	{
		for (int j = 0; j < map.map_size.width; j++)
		{
			if (!platforms[i][j].isWall)
			{
				platforms[i][j].set_color(255, 255, 255);
			}
		}
	}

	platforms = step_of_wave_search(platforms, start_point.x, start_point.y, finish_point.x, finish_point.y, window);
	platforms = find_way(platforms, finish_point.x, finish_point.y, window);
	std::vector<std::list<Platform>> list = platform_sort(platforms, map.map_size);
	bool show_way = false;



	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			resize_to_min(window);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		for (int i = 0; i < map.map_size.height; i++)
		{
			for (int j = 0; j < map.map_size.width; j++)
			{
				platforms[i][j].draw(window, sf::Color::White, sf::Color::Black);
			}
		}
		
		if (iteration <= list.size())
		{
			for (int i = 0; i < iteration; i++)
			{
				std::list<Platform> sub_list = list.at(i);

				for (Platform item: sub_list)
				{
					item.draw(window, sf::Color(item.color.red, item.color.green, item.color.blue), sf::Color::Black);
					draw_value(window, item);
				}
			}

			//Sleep(50);

			if (show_way)
			{
				draw_way(platforms, map.map_size, window);
			}

			if (iteration < list.size())
			{
				iteration += 1;
			}
			else
			{
				show_way = true;
			}
		}

		window.display();
	}
}


//main body
int main()
{
	Map map;
	map = load_map();
	draw_graphics(map);
	return 0;
}