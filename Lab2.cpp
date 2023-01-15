#include <iostream>
#include "AppWindow.h"
#include "Examples.h"
#include "Functions.h"
#include "imgui.h"
#include "Texture.h"
#include "MImageCalculator.h"

float  MyFunc(SDL_Point point)
{

	const SDL_Point circlePos1{ 800 / 2, 600 / 2 };
	const float circleRadius1 = std::min(800, 600) / 2;
	const SDL_Point circlePos2{ 800 / 2, 600 / 2 + 50 };
	const float circleRadius2 = std::max(800, 600) / 8;
	const SDL_Point circlePos3{ 800 / 2 - 100, 600 / 2 - 150 };
	const float circleRadius3 = std::min(800, 600) / 10;
	const SDL_Point circlePos4{ 800 / 2 + 100, 600 / 2 - 150 };
	const float circleRadius4 = std::min(800, 600) / 10;
	const SDL_Point EllipsePos1{ 800 / 2 + circleRadius1 / 2 + 200, 500 / 2 };
	const SDL_Point EllipsePos2{ 800 / 2 - circleRadius1 / 2 - 200, 500 / 2 };
	float c1 = Functions::Circle(circlePos1, circleRadius1, point) * 1000;
	float c2 = Functions::Circle(circlePos2, circleRadius2, point);
	float c3 = Functions::Circle(circlePos3, circleRadius3, point);
	float c4 = Functions::Circle(circlePos4, circleRadius4, point);
	float e1 = Functions::Ellips(EllipsePos1, 50, 100, point) * 1000;
	float e2 = Functions::Ellips(EllipsePos2, 50, 100, point) * 1000;
	float res = Functions::ROr(Functions::RAnd(c1, -Functions::ROr(c3, Functions::ROr(c2, c4))), Functions::ROr(e1, e2));
	return res;
};

class Window: public AppWindow
{
	void ComputeFunction()
{
	std::vector<std::vector<MImagePixelData>> data = calculator.GetSpaceData(texture.GetSize());
	for (size_t i = 0; i < data.size(); ++i)
	{
		for (size_t j = 0; j < data[i].size(); ++j)
		{
			MImagePixelData mimageData = data[i][j];
			uint8_t colorValue = (mimageData.nx + 1.f) * 127.f;
			if (mimageData.zone == FunctionZone::Positive)
				texture.SetPixel(SDL_Point(i, j), SDL_Color{colorValue, 0, 0, 255});
			else if (mimageData.zone == FunctionZone::Negative)
				texture.SetPixel(SDL_Point(i, j), SDL_Color{0, 0, colorValue, 255});
		}
	}
	texture.UpdateTexture();
}	
	void GradientDescent(SDL_Point startPoint, int step, SDL_Color pathColor = { 0, 255, 0, 255 })
	{
		// Спускаемся пока не выйдем за пределы пространства текстуры
		while (startPoint.x >= 0 && startPoint.x < texture.GetSize().x &&
			startPoint.y >= 0 && startPoint.y < texture.GetSize().y)
		{
			// Проверяем, не попали ли мы в точку, которая уже была рассчитана
			SDL_Color newColor = texture.GetColor({ startPoint.x, startPoint.y });
			if (newColor.r == pathColor.r &&
				newColor.g == pathColor.g &&
				newColor.b == pathColor.b &&
				newColor.a == pathColor.a)
			{
				break;
			}

			// Окрашивание пути
			texture.SetPixel({ startPoint.x, startPoint.y }, pathColor);

			// Рассчет ЛГХ в текущей точке
			MImagePixelData data = calculator.GetPixelData({ startPoint.x, startPoint.y });
			// Движение по направлению убывания с шагом
			startPoint.x -= step * data.nx;
			startPoint.y += step * data.ny;
		}
		texture.UpdateTexture();
	}
public:
	Window(SDL_Point windowSize):
		AppWindow(windowSize),
		texture(GetRenderer(), windowSize),
		calculator(&MyFunc) 
	{
		ComputeFunction();
	}
	
	void Render() override
	{
		texture.Render();
	}
	
	void RenderGui() override
	{
		ImGui::Begin("MyWindow");
		ImGui::Text("Mouse position");
		ImGui::Text("\t%d, %d", mousePosition.x, mousePosition.y); // Aka printf
		ImGui::Text("Mouse MImage Data");
		ImGui::Text("\t nx = %f, ny = %f", calculator.GetPixelData(mousePosition).nx, calculator.GetPixelData(mousePosition).ny); // Aka printf
		ImGui::End();
	}
	
	void ProcessEvent(const SDL_Event& e) override
	{
		if (e.type == SDL_MOUSEMOTION)
		{
			mousePosition.x = e.motion.x;
			mousePosition.y = e.motion.y;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				SDL_Point startPoint;
				startPoint.x = e.button.x;
				startPoint.y = e.button.y;
				GradientDescent(startPoint, 2);
			}
		}
	}
	MImageCalculator calculator;
	Texture texture;
	SDL_Point mousePosition;
	
};


int main(int argc, char** argv)
{
	Window window({800, 600});
	
	window.Show();
	
	return 0;
}