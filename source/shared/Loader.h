#pragma once

#include "../shared/VecGrid.h"
#include "../engine/Handle.h"
#include "../engine/String.h"

class ScriptReader;
class ScriptLine;
class Entity;

//=============================================================================
//	Loader
//=============================================================================

class Loader
{
	public:
		virtual ~Loader() = default;

		bool Load(const String&);

	protected:
		virtual Handle<Entity> CreateEntity(const String&) = 0;
		virtual void OnLoadEnd() {}
		virtual bool Load_Partition(ScriptReader*, uint& line) { return false; }
};

//=============================================================================
//	EditorLoader
//=============================================================================

class EditorLoader : public Loader
{
	protected:
		virtual Handle<Entity> CreateEntity(const String&)override final;
		virtual void OnLoadEnd()override final;
};

//=============================================================================
//	GameLoader
//=============================================================================

class GameLoader : public Loader
{
	protected:
		virtual Handle<Entity> CreateEntity(const String&)override final;
		void CreatePartition(const VecGrid& pos, const VecGrid& scale, bool debugAddEntity, const String& debugEntityContact);
		void OnLoadEnd()override final;
		bool Load_Partition(ScriptReader*, uint& line)override final;
};