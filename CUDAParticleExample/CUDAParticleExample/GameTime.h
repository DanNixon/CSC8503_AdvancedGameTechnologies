#pragma once

class GameTime {
public:
	static void startTime();
	static void startFrame();
	static double getFrameTime();
	static double getTime();
	static long long getCycles();
protected:
	static long long start_cycles;
	static long long frame_start_cycles;
	static double frame_time;
	static long long cycles_per_sec;
};