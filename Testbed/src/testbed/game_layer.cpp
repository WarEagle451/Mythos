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

void game_layer::on_event(event::event_base& a_event) {
	event::dispatcher dispatcher(a_event);
}

void game_layer::update(timestep ts) {
	m_scene.update(ts);
}

void game_layer::render() {
	m_scene.render();
}