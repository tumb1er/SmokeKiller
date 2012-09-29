#pragma once
enum State {OPEN, HALFOPEN, OPENING, STEPOPEN, OPENING_MOVE, CLOSED, CLOSED_MOVE, CLOSING, STEPCLOSE};

class StateController {
	State state;

}