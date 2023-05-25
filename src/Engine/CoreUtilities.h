#pragma once

template<typename T, typename L>
bool RemoveElement(L& list, T& object, bool uniqueObject = true)
{
	if (!object) return false;

	bool result = false;
	for (auto it = list.begin(); it != list.end();)
	{
		if (*it == object)
		{
			result = true;
			object.reset();
			it = list.erase(it);
			if (uniqueObject) break;
		}
		else
			++it;
	}

	return result;
}

template <class T>
bool RemoveElement(std::list<T>& list, T& object, bool uniqueObject = true)
{
	return RemoveElement<T, std::list<T>>(list, object, uniqueObject);
}