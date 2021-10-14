#pragma once
class FPSLimiter {
public:
	FPSLimiter(int limit = -1);

	void setLimit(int limit = -1);
	void setQueueSize(std::size_t size = 10);

	void update();
	float getFPS() const;

private:
	std::chrono::nanoseconds waitingTime{ 0 };
	std::queue<std::chrono::steady_clock::time_point> times;
	size_t queueSize = 10;
};

