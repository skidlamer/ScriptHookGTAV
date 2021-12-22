#pragma once
// A inheritable class to disallow the copy constructor and operator = functions
class NonCopyable
{
protected:
	NonCopyable() {}
	NonCopyable(const NonCopyable&&)noexcept {}
	void operator=(const NonCopyable&&)noexcept {}
private:
	NonCopyable(NonCopyable&);
	NonCopyable& operator=(NonCopyable& other) = delete;
};