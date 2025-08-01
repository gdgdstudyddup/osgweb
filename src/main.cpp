
#include <emscripten.h>
#include <SDL2/SDL.h>

#include "Application.h"
// We use app global variable in loop() function.
Application* app = 0;
// Stand alone function that is called by Emscripten to run the app.
void loop()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (app)
		{
			app->handleEvent(e);
		}
	}
	if (app)
	{
		app->frame();
	}
}
extern "C" {

	// void EMSCRIPTEN_KEEPALIVE Foo()
	// {
	//     if(app)
	//     {
	//         app->loadScene("res/box.osgt");
	//     } 
	// }
    int extern custom_add(int a, int b);
	int EMSCRIPTEN_KEEPALIVE SumJSArray(int* arr, int size)
	{
		int sum = 0;
		for (int i = 0; i < size; i++)
		{
			//printf("SumJSArray: arr[%d] = %d\n", i, arr[i]);
			sum = sum + arr[i];
		}
		return sum;
	}
	void* EMSCRIPTEN_KEEPALIVE MallocMemory(size_t size)
	{
		return malloc(size);
	};

	void EMSCRIPTEN_KEEPALIVE FreeMemory(void* ptr)
	{
		free(ptr);
	}

	void EMSCRIPTEN_KEEPALIVE CreateNodes(const char* namesfromJs, int* sizeArray, int* shapeSizeArray, int lengthOfSizeArray, const char* parentNamefromJs, int op)
	{
		if (app)
		{
			app->createNodes(namesfromJs, sizeArray, shapeSizeArray, lengthOfSizeArray, parentNamefromJs, op);
		}
	}
	void EMSCRIPTEN_KEEPALIVE UpdateNodes(const char* searchName)
	{
		if (app)
		{
			app->updateNodes(searchName);
		}
	}
	void EMSCRIPTEN_KEEPALIVE ClearNodes()
	{
		if(app)
        {
            app->clearNodes();
        }
	}
    void EMSCRIPTEN_KEEPALIVE SetTime(float time)
    {
        global::time = time;
    }
	void EMSCRIPTEN_KEEPALIVE TestAPI()
	{
		app->test();
	}
	void EMSCRIPTEN_KEEPALIVE Home()
	{
		if (app)
		{
			app->home();
		}
	}
	void EMSCRIPTEN_KEEPALIVE CameraIsometric()
	{
		if (app)
		{
			app->cameraPositionIsometric();
		}
	}
	void EMSCRIPTEN_KEEPALIVE CameraSide()
	{
		if (app)
		{
			app->cameraPositionSide();
		}
	}
	void EMSCRIPTEN_KEEPALIVE CameraFront()
	{
		if (app)
		{
			app->cameraPositionFront();
		}
	}
	void EMSCRIPTEN_KEEPALIVE CameraTop()
	{
		if (app)
		{
			app->cameraPositionTop();
		}	
	}
	void EMSCRIPTEN_KEEPALIVE RotateFixed(int isLeft)
	{
		if(app)
		{
			app->rotateFixed(isLeft);
		}
	}
	void EMSCRIPTEN_KEEPALIVE setPressCtrl(bool isPressCtrl)
	{
		if(app)
		{
			app->setPressCtrl(isPressCtrl);
		}
	}
	void EMSCRIPTEN_KEEPALIVE SyncModelInfo(
		const char* namefromJs,
		float* position,
		float* normal,
		float* color,
		int lengthOfVertex,
		int* indices,
		int lengthOfIndices,
		unsigned char* imageData,
		int width,
		int height,
		int channel
	)
	{
		if(app)
		{
			app->addModelData(
				namefromJs,
				position,
				normal,
				color,
				lengthOfVertex,
				indices,
				lengthOfIndices,
				imageData,
				width,
				height,
				channel
			);
		}
			// std::string modelName(namefromJs);
			// osg::Vec3 myCoords[] =
            // {
            //     osg::Vec3(-1.12056, -2.15188e-09, -0.840418),
            //     osg::Vec3(-0.95165, -2.15188e-09, -0.840418),
            //     osg::Vec3(-1.11644, 9.18133e-09, -0.716827),

            //     // note in anticlockwise order.
            //     osg::Vec3(-0.840418, 9.18133e-09, -0.778623),
            //     osg::Vec3(-0.622074, 9.18133e-09, -0.613835),
            //     osg::Vec3(-1.067, 9.18133e-09, -0.609715),

            // };


            // int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

			// basePlane = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			// this->setVertexArray(vertices);
			// this->setNormalArray(normals);
			// this->setTexCoordArray(0,texcoords);
			// this->setNormalBinding( osg::Geometry::BIND_PER_VERTEX);
	}
	// shapeFileUrl
	/* 
	@param namefromJs: The name of the Node.
	@param op: Operation type, 0 for update, 1 for create.
	@param data: Pointer to the float array containing transform data.
	@param lengthArray: Pointer to an integer array containing the lengths of each transformData.
	@params stepSize  0.002,
	@param intervals  500,
	@param startTime  0,
	@param stopTime  1
	*/
	void EMSCRIPTEN_KEEPALIVE SyncAnimateData(
		const char* namefromJs,
		int op,
		float stepSize,
		int intervals,
		float startTime,
		float stopTime,
		float* data,
		int lengthArray)
	{
		const int ARRAY_ATTRIBUTE_COUNT = 11;
		if (app)
		{
			size_t len = strlen(namefromJs);
			char* temp = (char*)malloc(len + 1);
			strcpy(temp, namefromJs);
			std::string name(temp);
			free((void*)temp);
			using TransformData = global::TransformData;
			std::map<std::string, TransformData* >& transformDataMap = global::transformDataMap;
			std::cout << "Map size before: " << transformDataMap.size() << std::endl;
			if (op != 0)
			{
				transformDataMap.clear();
			}
			// we cannot use op=1 to clear transformMap because we will initialize map after clearing. use ClearAnimateData instead.
			//std::cout<<name<<"\n";
			if (transformDataMap.find(name) == transformDataMap.end())
			{
				std::cout<<name<<" does not be found\n";
				transformDataMap[name] = new TransformData();

			}
			int idx = 0;
			for(int j=0; j < lengthArray; j++)
			{
				for (int z = 0; z < 9; z++)
				{
					
					transformDataMap[name]->basis.push_back(data[idx++]);
				}
				for (int z = 0; z < 3; z++)
				{
					
					transformDataMap[name]->rotation.push_back(data[idx++]);
				}
				for (int z = 0; z < 3; z++)
				{
					
					transformDataMap[name]->lengthDir.push_back(data[idx++]);
				}
				
				transformDataMap[name]->length.push_back(data[idx++]);
				
				transformDataMap[name]->width.push_back(data[idx++]);
				
				transformDataMap[name]->height.push_back(data[idx++]);
				for (int z = 0; z < 3; z++)
				{
					
					transformDataMap[name]->color.push_back(data[idx++]/255.0);
				}
				transformDataMap[name]->specCoeff.push_back(data[idx++]);
				for (int z = 0; z < 3; z++)
                {
					
                    transformDataMap[name]->widthDir.push_back(data[idx++]);
                }
				for (int z = 0; z < 3; z++)
                {
					
                    transformDataMap[name]->rShape.push_back(data[idx++]);
                }
				
				transformDataMap[name]->extra.push_back(data[idx++]);
			}
			//std::cout<<transformDataMap.size()<<" transformDataMap size."<<std::endl;
            
		}

	}
    void EMSCRIPTEN_KEEPALIVE Init(int width, int height, int r, int g, int b, int a, int useLogger)
    {   
        // Make sure SDL is working.
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("OSGWeb. Could not init SDL: '%s'\n", SDL_GetError());
            return ;
        }
        // printf("call js func return %d\n", custom_add(2, 3));
        // Clean SDL up at exit.
        atexit(SDL_Quit);
        // Configure rendering context.
        // SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        // SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        // SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // enable MULTISAMPLE
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // can be 2, 4, 8 or 16
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        // Create rendering window.
        // int width = 800;
        // int height = 600;
        SDL_Window* window =
            SDL_CreateWindow(
                "OSGWeb",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                width,
                height,
                SDL_WINDOW_OPENGL);
        if (!window)
        {
            printf("OSGWeb. Could not create window: '%s'\n", SDL_GetError());
            return ;
        }
        SDL_GL_CreateContext(window);
        // Create application.
        app = new Application;
        app->setupWindow(width, height);
        app->setBackgroundColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        if(useLogger)
        {
            app->setupLogging();
        }
        emscripten_set_main_loop(loop, -1, 0);
    }
	void EMSCRIPTEN_KEEPALIVE ClearAnimateData()
	{
		using TransformData = global::TransformData;
		std::map<std::string, TransformData* >& transformDataMap = global::transformDataMap;
		//transformDataMap.clear();
		std::map<std::string, TransformData* >::iterator it;
		for(it=transformDataMap.begin(); it!=transformDataMap.end();)
		{
			//cout<<"key: "<<it->first <<" value: "<<*it->second<<endl;
			delete it->second;
			transformDataMap.erase(it++);
		}
		std::cout << "OSGWeb. Clear animate data." << std::endl;
	}
    void EMSCRIPTEN_KEEPALIVE Destroy()
    {
        std::cout << "OSGWeb. Destroying application." << std::endl;
        emscripten_cancel_main_loop();
        if(app)
        {
            std::cout << "OSGWeb. Deleting application." << std::endl;
            delete app;
            app = nullptr;
            std::cout << "OSGWeb. Application deleted." << std::endl;
        }
    }
    void EMSCRIPTEN_KEEPALIVE Resize(int width, int height)
    {
        if (app)
        {
            app->resizeWindow(width, height);
        }
    }
}



int main(int argc, char* argv[])
{
	
	// app->loadScene("res/box.osgt");
	// Render asynchronously.
	
    // emscripten_cancel_main_loop
	/**
	 * https://discourse.libsdl.org/t/how-to-properly-resize-and-redraw-after-window-resize-event/41253/3
	 * if(resized) {
		resized = false;
		SDL_GetWindowSurface(window, &width, &height);
		surface = SDL_GetWindowSurface(window);
		uint32_t black = SDL_MapRGBA(surface->format, 0, 0, 0, 255);
		SDL_FillRect(surface, NULL, black);        // clear the screen
		SDL_BlitSurface(image, NULL, surface, NULL);
		SDL_UpdateWindowSurface(window);
	}
	 */
	return 0;
}

