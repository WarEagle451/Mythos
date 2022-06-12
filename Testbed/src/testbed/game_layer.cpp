#include "game_layer.hpp"

using namespace myl;

game_layer::game_layer()
	: layer("Game") {

}

game_layer::~game_layer() {

}

void game_layer::on_attach() {

}

void game_layer::on_detach() {

}

void game_layer::on_event(event& a_event) {
	event_dispatcher dispatcher(a_event);
}

void game_layer::update(timestep) {
	m_scene.update();
}

void game_layer::render() {
	m_scene.render();
}