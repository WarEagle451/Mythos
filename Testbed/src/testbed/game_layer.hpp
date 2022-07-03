#pragma once
#include <mythos/scene/scene.hpp>
#include <mythos/core/layer.hpp>

class game_layer : public myl::layer {
	myl::scene m_scene;
public:
	game_layer();
	~game_layer();

	void on_attach() override;
	void on_detach() override;
	void on_event(myl::event::event_base&) override;
	void update(myl::timestep) override;
	void render() override;
};