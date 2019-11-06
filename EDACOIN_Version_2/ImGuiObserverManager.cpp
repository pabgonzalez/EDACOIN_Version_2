#include "ImGuiObserverManager.h"

ImGuiObserverManager::ImGuiObserverManager() {
	display = NULL;
	error = true;
	if (allegroInit()) {
		error = false;

		eventQueue = al_create_event_queue();
		if (eventQueue != NULL)
		{
			al_register_event_source(eventQueue, al_get_keyboard_event_source());
			al_register_event_source(eventQueue, al_get_mouse_event_source());
			al_register_event_source(eventQueue, al_get_display_event_source(display));
		}

	}
}

ImGuiObserverManager::~ImGuiObserverManager() {
	if (error == false) {
		al_unregister_event_source(eventQueue, al_get_display_event_source(display));
		al_destroy_event_queue(eventQueue);
		allegroDestroy();
	}
}

void ImGuiObserverManager::addObserver(Observer* o) {
	observers.push_back(o);
}

void ImGuiObserverManager::removeObserver(int i) {
	observers.erase(observers.begin()+i);
}

void ImGuiObserverManager::cycle() {
	if (al_get_next_event(eventQueue, &ev)) {
		ImGui_ImplAllegro5_ProcessEvent(&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			ev.type = FIN_IMGUI;
			return;
		}
	}

	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	for (int i = 0; i < observers.size(); i++) {
		observers[i]->cycle();
	}

	// Rendering
	ImGui::Render();
	al_clear_to_color(al_map_rgba_f(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

	al_flip_display();
}


bool ImGuiObserverManager::allegroInit()
{
	if (al_init())
	{
		if (al_init_primitives_addon())
		{
			if (al_install_keyboard())
			{
				if (al_install_mouse())
				{
					if (al_init_font_addon())
					{
						if (al_init_ttf_addon())
						{
							display = al_create_display(1200, 600);
							if (display != NULL)
							{
								al_set_window_title(display, "EDA-Coin");
								IMGUI_CHECKVERSION();
								ImGui::CreateContext();
								ImGuiIO& io = ImGui::GetIO(); (void)io;
								//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
								ImGui::StyleColorsDark();
								ImGui_ImplAllegro5_Init(display);
								return true;
							}
							else
								cout << "Unable to create display" << endl;
							al_shutdown_ttf_addon();
						}
						else
							cout << "Unable to init ttf font addon" << endl;
						al_shutdown_font_addon();
					}
					else
						cout << "Unable to init font addon" << endl;
					al_uninstall_mouse();
				}
				else
					cout << "Unable to install mouse" << endl;
				al_uninstall_keyboard();
			}
			else
				cout << "Unable to install keyboard" << endl;
			al_shutdown_primitives_addon();
		}
		else
			cout << "Unable to init primitives addon" << endl;
	}
	else
		cout << "Unable to init Allegro" << endl;
	return false;
}

void ImGuiObserverManager::allegroDestroy()
{
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	al_destroy_display(display);
	al_shutdown_font_addon();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_primitives_addon();
}
