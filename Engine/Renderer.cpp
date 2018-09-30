/*
MIT License

Copyright (c) 2018 Ben Brown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Renderer.hpp"
#include "DriverD3D12.hpp"
#include "DriverVk.hpp"
#include "thirdparty/loguru.hpp"

#include <SDL2/SDL.h>

Renderer::Renderer(RendererDriver driver) : m_Driver(driver) {}

Renderer::~Renderer() {
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

bool Renderer::initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	m_pWindow = SDL_CreateWindow("Ivy3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (!m_pWindow) {
		LOG_F(FATAL, "SDL window is invalid!");
		SDL_Quit();
		return false;
	}

	// TODO: If autodetect is enabled, we will need to enumerate 
	// both drivers and select the best one.
	if (m_Driver == RendererDriver::eDirect3D12) {
		m_pDriver = std::make_unique<DriverD3D12>(m_pWindow);
		LOG_F(INFO, "Direct3D12 driver was selected.");
	} else if (m_Driver == RendererDriver::eVulkan) {
		m_pDriver = std::make_unique<DriverVk>(m_pWindow);
		LOG_F(INFO, "Vulkan driver was selected.");
	} else
		return false;

    if (!m_pDriver->initialize()) {
        LOG_F(FATAL, "Failed to initialize render driver!");
        return false;
    }

    auto gpus = m_pDriver->getGpus();
    if (!m_pDriver->selectGpu(gpus.front().id)) {
		LOG_F(FATAL, "Failed to select GPU for operation!");
        return false;
    }
	m_Running = true;
    return true;
}

RendererDriver Renderer::getRendererDriver() {
    return m_Driver;
}

bool Renderer::setRendererDriver(RendererDriver driver) {
    if (driver != m_Driver) {
        m_pDriver.reset();
    }
    return false;
}

bool Renderer::getVsync() {
    return m_Vsync;
}

void Renderer::setTextureFiltering(TextureFiltering textureFiltering) {
    m_TextureFiltering = textureFiltering;
}

void Renderer::onKeyPress() {}

int Renderer::executeEventLoop() {
	while (m_Running) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != false) {
			// Dispatch appropriate callback
			switch (event.type) {
			case SDL_KEYDOWN: onKeyPress(); break;
			case SDL_QUIT: shutdown(); break;
			}
		}
	}

	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
	return 0;
}

void Renderer::shutdown() {
	m_Running = false;
}

TextureFiltering Renderer::getTextureFiltering() {
    return m_TextureFiltering;
}
