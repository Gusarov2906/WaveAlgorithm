#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string> 
#include <cstdlib>


#define CharacterSize 24
#define RectangleOutline 2
#define WindowWidth 1026
#define WindowHeight 770
#define RectangleWidth 128
#define RectangleHeight 64
#define MaxMapSizeX 8
#define MaxMapSizeY 8


//structure for save current map size
struct MapSize
{
public:
	int width;
	int height;
};

//structure to return from load func
struct Map
{
public:
	MapSize map_size;
	char** array;
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
	}

	//draw platform
	void draw(sf::RenderWindow &window)
	{
		sf::RectangleShape rectangle(sf::Vector2f(size_x- RectangleOutline, size_y- RectangleOutline));
		if (not isWall)
		{
			//white objects are road
			rectangle.setFillColor(sf::Color::White);
		}
		else
			//black objects are wall
			rectangle.setFillColor(sf::Color::Black);

		rectangle.setOutlineThickness(RectangleOutline);
		rectangle.setOutlineColor(sf::Color::Black);
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
	}

	void set_finish_point()
	{
		isFinishPoint = true;
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

//block of code for graphics
void draw_graphics(Map map)
{
	sf::RenderWindow window(sf::VideoMode(map.map_size.width*RectangleWidth+RectangleOutline, map.map_size.height*RectangleHeight+RectangleOutline), "Wave_alg", sf::Style::Default);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (window.getSize().x < sf::Vector2u(1026, 770).x)
				window.setSize(sf::Vector2u(1026, window.getSize().y));
			if (window.getSize().y < sf::Vector2u(1026, 770).y)
				window.setSize(sf::Vector2u(window.getSize().x, 770));
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		Platform** platforms = new Platform*[map.map_size.height];
		for (int i = 0; i < map.map_size.height; i++)
		{
			platforms[i] = new Platform[map.map_size.width];
			for (int j = 0; j < map.map_size.width; j++)
			{
				platforms[i][j] = Platform(i, j, RectangleWidth * j, RectangleHeight * i, RectangleWidth, RectangleHeight);
				if (map.array[i][j] == '1')
					platforms[i][j].set_wall();
				else if (map.array[i][j] == 'A')
					platforms[i][j].set_start_point();
				else if (map.array[i][j] == 'B')
					platforms[i][j].set_finish_point();
				platforms[i][j].draw(window);
			}
		}
		/*
		platforms[2].set_start_point();
		platforms[2].draw(window);
		platforms[80].set_finish_point();
		platforms[80].draw(window);
		for (int i = 10; i < 14; i++)
		{
			platforms[i].set_wall();
			platforms[i].draw(window);
		}
		*/

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