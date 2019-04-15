#pragma once

#include "../engine/Math.h"
#include "../engine/Pointer.h"
#include "../engine/EventListener.h"
#include "../shared/VecGrid.h"

class Model;
class Shader_OneColor;
struct Event;
class IGridLockAction;

class Grid : public EventListener
{
	public:
		Grid();
		virtual ~Grid();

		void Shutdown();
		void Initialize();
		void Update(const math::CameraTransform&);
		void Render(const math::CameraTransform&);

		uint GetCurrentLevel()const;
		// TODO: rename to conform to standard
		static float GetSizeLevel(uint);
		float GetSizeCurrentLevel()const;
		static uint GetUnitLevel(uint);
		uint GetUnitCurrentLevel()const;

		static VecGrid ToVecGrid(const math::Vec3&);
		static math::Vec3 ToVec3(const VecGrid&);

		int SnapToGrid(int value, math::eSign, uint units)const;
		VecGrid SnapToGrid(const math::Vec3&, uint units)const;

		void ShowGrid(bool show) { m_show = show; }
		void ToggleGrid() { m_show = !m_show; }
		bool IsGridVisible()const { return m_show; }

		void HandleEvent(Event*)override;

	private:
		bool m_initialized;
		bool m_show;
		SimplePtr<Model> m_pModel;
		SimplePtr<Shader_OneColor> m_pShader;
		SimplePtr<IGridLockAction> m_pGridLockAction;
		uint m_level;
};
