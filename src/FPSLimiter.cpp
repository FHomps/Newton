#include "stdafx.h"
#include "FPSLimiter.h"

FPSLimiter::FPSLimiter(int limit) {
	setLimit(limit);
	times.push(std::chrono::steady_clock::now());
}

void FPSLimiter::setLimit(int limit) {
	if (limit > 0) {
		waitingTime = std::chrono::nanoseconds((long long)(1e9 / (double)limit));
	}
}

void FPSLimiter::setQueueSize(std::size_t size) {
	queueSize = size;
}

void FPSLimiter::update() {
	std::this_thread::sleep_until(times.back() + waitingTime);
	times.push(std::chrono::steady_clock::now());
	if (times.size() > queueSize)
		times.pop();
}

float FPSLimiter::getFPS() const {
	float fps = 1e9f * (times.size() - 1) / (float)(std::chrono::duration_cast<std::chrono::nanoseconds>(times.back() - times.front()).count());
	return fps;
}
