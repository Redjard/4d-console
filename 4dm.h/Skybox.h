#pragma once

#include "4dm.h"

namespace fdm 
{
	class Skybox : public Mesh 
	{
	public:
		inline static Skybox* instance = reinterpret_cast<Skybox*>((base + 0x279138));
		inline static const int VERTEX_COUNT = 36; 
		inline static glm::vec3* verts = reinterpret_cast<glm::vec3*>((base + 0x2784D0));
		inline static glm::vec2* uv = reinterpret_cast<glm::vec2*>((base + 0x278680));

		const void* buffData(int buffIndex) const override
		{
			return reinterpret_cast<const void* (__thiscall*)(const Skybox* self, int buffIndex)>(getFuncAddr((int)Func::Skybox::buffData))(this, buffIndex);
		}
		int buffSize(int buffIndex) const override
		{
			return reinterpret_cast<int (__thiscall*)(const Skybox* self, int buffIndex)>(getFuncAddr((int)Func::Skybox::buffSize))(this, buffIndex);
		}
		int attrSize(int buffIndex, int attrIndex) const override
		{
			return reinterpret_cast<int (__thiscall*)(const Skybox* self, int buffIndex, int attrIndex)>(getFuncAddr((int)Func::Skybox::attrSize))(this, buffIndex, attrIndex);
		}
		int vertCount() const override
		{
			return reinterpret_cast<int (__thiscall*)(const Skybox* self)>(getFuncAddr((int)Func::Skybox::vertCount))(this);
		}
	};
}
