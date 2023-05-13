#pragma once

class IApp
{
public:
	virtual ~IApp() = default;

	virtual bool Create() = 0;
	virtual void Destroy() = 0;

	virtual void Render() = 0;
	virtual void Update(float deltaTime) = 0;
};