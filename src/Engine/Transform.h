#pragma once

class Transform
{
public:
	void SetPosition(const glm::vec3& position)
	{
		m_translationDirty = true;
		m_position = position;
	}

	void SetEulerAngles(const glm::vec3& eulerAngles)
	{
		m_rotationDirty = true;
		m_eulerAngles.x = wrapAngle(eulerAngles.x);
		m_eulerAngles.y = wrapAngle(eulerAngles.y);
		m_eulerAngles.z = wrapAngle(eulerAngles.z);
	}

	void Translate(const glm::vec3& translation)
	{
		m_translationDirty = true;
		m_position += translation;
	}

	void SetRotation(const glm::quat& quaternion)
	{
		m_rotationDirty = true;
		m_eulerAngles.x = pitch(quaternion);
		m_eulerAngles.y = yaw(quaternion);
		m_eulerAngles.z = roll(quaternion);
	}

	void RotateX(const float radians)
	{
		m_rotationDirty = true;
		m_eulerAngles.x = wrapAngle(m_eulerAngles.x + radians);
	}

	void RotateY(const float radians)
	{
		m_rotationDirty = true;
		m_eulerAngles.y = wrapAngle(m_eulerAngles.y + radians);
	}

	void RotateZ(const float radians)
	{
		m_rotationDirty = true;
		m_eulerAngles.z = wrapAngle(m_eulerAngles.z + radians);
	}

	void ClampPitch()
	{
		m_rotationDirty = true;
		m_eulerAngles.x = clampPitch(m_eulerAngles.x);
	}

	[[nodiscard]] const glm::vec3& GetPosition() const { return m_position; }
	[[nodiscard]] const glm::vec3& GetEulerAngles() const { return m_eulerAngles; }

	[[nodiscard]] const glm::mat4& GetTranslationMatrix() const
	{
		if( m_translationDirty )
		{
			static constexpr glm::mat4 IDENTITY(1.0f);
			m_translationMatrix = translate(IDENTITY, m_position);
			m_translationDirty = false;
		}
		return m_translationMatrix;
	}

	[[nodiscard]] const glm::mat4& GetRotationMatrix() const
	{
		if( m_rotationDirty )
		{
			m_rotationMatrix = glm::eulerAngleYXZ(m_eulerAngles.y, m_eulerAngles.x, m_eulerAngles.z);
			m_rotationDirty = false;
		}
		return m_rotationMatrix;
	}

	[[nodiscard]] glm::vec3 GetRightVector() const
	{
		static constexpr glm::vec4 RIGHT{ 1, 0, 0, 0 };
		return GetRotationMatrix() * RIGHT;
	}

	[[nodiscard]] glm::vec3 GetUpVector() const
	{
		static constexpr glm::vec4 UP{ 0, 1, 0, 0 };
		return GetRotationMatrix() * UP;
	}

	[[nodiscard]] glm::vec3 GetForwardVector() const
	{
#ifndef GLM_FORCE_LEFT_HANDED // TODO: удалить
		static constexpr glm::vec4 FORWARD{ 0, 0, -1, 0 };
#else
		static constexpr glm::vec4 FORWARD{ 0, 0, 1, 0 };
#endif
		return GetRotationMatrix() * FORWARD;
	}

	[[nodiscard]] glm::vec3 GetHorizontalRightVector() const
	{
		const float yaw = m_eulerAngles.y;
		return { glm::cos(yaw), 0, -glm::sin(yaw) };
	}

	[[nodiscard]] glm::vec3 GetHorizontalForwardVector() const
	{
		const float yaw = m_eulerAngles.y;
#ifndef GLM_FORCE_LEFT_HANDED // TODO: удалить
		return { -glm::sin(yaw), 0, -glm::cos(yaw) };
#else
		return { glm::sin(yaw), 0, glm::cos(yaw) };
#endif
	}

	[[nodiscard]] glm::mat4 GetMatrix() const
	{
		return GetTranslationMatrix() * GetRotationMatrix();
	}

	[[nodiscard]] glm::mat4 GetInverseMatrix() const
	{
		return inverse(GetMatrix());
	}

private:
	static float clampPitch(const float radians) { return glm::clamp(radians, -glm::half_pi<float>(), glm::half_pi<float>()); }

	// Wrap to (-PI..PI]
	static float wrapAngle(const float radians)
	{
		return std::remainder(radians, glm::two_pi<float>());
	}

	glm::vec3 m_position{};
	glm::vec3 m_eulerAngles{}; // in radians

	mutable bool      m_translationDirty = true;
	mutable glm::mat4 m_translationMatrix{ 1.0f };

	mutable bool      m_rotationDirty = true;
	mutable glm::mat4 m_rotationMatrix{ 1.0f };
};