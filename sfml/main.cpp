#include <SFML/Graphics.hpp>
#include <ctime> 
#include <windows.h>

int cellsize = 10, w, h;
bool pause, nextFrame;
bool** grid_current;
bool** grid_next;
sf::VideoMode DesktopMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(DesktopMode, "Conways game of life", sf::Style::Fullscreen);

void DrawGrid(sf::Vertex line[], int width, int height)
{
    int ky = height % cellsize;
    int kx = width % cellsize;
    width += 1;
    height += 1;

    for (int i = 1; i <= width; i += cellsize)
    {
        line[0].position.x = i;
        line[0].position.y = 1;
        line[1].position.x = i;
        line[1].position.y = height-ky;
        window.draw(line, 2, sf::Lines);
    }

    for (int i = 1; i <= height; i += cellsize)
    {
        line[0].position.x = 1;
        line[0].position.y = i;
        line[1].position.x = width-kx;
        line[1].position.y = i;
        window.draw(line, 2, sf::Lines);
    }
}

int surrounding(bool** grid, int x, int y)
{
    int count = 0;

    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
			if (grid[(i + w) % w][(j + h) % h])
				count++;
        }
    }

    if (grid[x][y])
        count -= 1;

    return count;
}

void HideConsole()
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
}

int GetCoreCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

bool vsync = false;
void ToggleVsync()
{
    vsync = !vsync;
    window.setVerticalSyncEnabled(vsync);
}

void ClearGrid(bool** grid, int w, int h)
{
    for (int i = 0; i < w; i++)
    {
        memset(grid[i], 0, sizeof(bool) * h);
    }
}

void DrawNextGen(sf::RectangleShape rectangle)
{
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            if (grid_current[i][j])
            {
                rectangle.setPosition(i * cellsize + 1, j * cellsize + 1);
                window.draw(rectangle);
            }

            if (!pause || nextFrame) {
                int count = surrounding(grid_current, i, j);

                if (count < 2)
                    grid_next[i][j] = false;
                else if (count == 3)
                    grid_next[i][j] = true;
                else if (count > 3)
                    grid_next[i][j] = false;
                else
                    grid_next[i][j] = grid_current[i][j];

            }
        }
    }
}

void UpdateGrid(int w, int h, bool pause, bool nextFrame, bool**& grid_current, bool**& grid_next)
{
	if (!pause || nextFrame) {
		bool** temp = grid_current;
		grid_current = grid_next;
		grid_next = temp;

		ClearGrid(grid_next, w, h);
	}
}


int main()
{
    HideConsole();
    srand(time(NULL));

    ToggleVsync();

	sf::RectangleShape rectangle(sf::Vector2f(cellsize-1, cellsize-1));
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition(0, 0);

    
	sf::Font f;
    f.loadFromFile("c:\\Windows\\Fonts\\arial.ttf");
    
	sf::Text text;
    text.setFont(f);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);

    int x = window.getSize().x;
    int y = window.getSize().y;

    w = x / cellsize;
    h = y / cellsize;

    pause = true;
    nextFrame = false;

    grid_current = new bool* [w];
    grid_next = new bool* [w];
    for (int i = 0; i < x / cellsize; i++)
    {
        grid_current[i] = new bool[h];
        grid_next[i] = new bool[h];

        memset(grid_current[i], false, sizeof(bool)*h);
        memset(grid_next[i], false, sizeof(bool) * h);
    }    

    

    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(10.f, 10.f)),
        sf::Vertex(sf::Vector2f(150.f, 150.f))
    };
    line[0].color.r = 100;
    line[0].color.g = 100;
    line[0].color.b = 100;
    line[1].color.r = 100;
    line[1].color.g = 100;
    line[1].color.b = 100;

    window.setFramerateLimit(0);
    sf::Clock Clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    pause = !pause;
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    nextFrame = true;
                }

                if (event.key.code == sf::Keyboard::R)
                {
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            grid_current[i][j] = rand() % 2;
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::C)
                {
                    ClearGrid(grid_current, w, h);
                }

                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();

                    for (int i = 0; i < w; i++)
                    {
                        delete grid_current[i];
                        delete grid_next[i];
                    }
                    delete[] grid_current;
                    delete[] grid_next;
                    return 0;
                }

                if (event.key.code == sf::Keyboard::V)
                {
                    ToggleVsync();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int i = event.mouseButton.x / cellsize;
                    int j = event.mouseButton.y / cellsize;
                    grid_current[i][j] = !grid_current[i][j];
                }
            }
        }

        window.clear();
        float Time = Clock.getElapsedTime().asMicroseconds();
        Clock.restart();
        std::string fps = std::to_string(static_cast<int>(1000000.0 / Time));
        text.setString(fps + "fps");

        DrawGrid(line, x, y);

        DrawNextGen(rectangle);
            
        UpdateGrid(w, h, pause, nextFrame, grid_current, grid_next);
        nextFrame = false;
        window.draw(text);
        window.display();
    }

    return 0;
}
