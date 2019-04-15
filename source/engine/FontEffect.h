#pragma once

#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/String.h"

class Font;
class Effect_Text2d;
class Model;
struct BitmapCharInfo;

/****************************************************************************************
	FontEffect
*****************************************************************************************/

class FontEffect
{
	public:
		virtual ~FontEffect();

		void Initialize();
		void Shutdown();

		void SetCharSize(uint f) { m_size = f; }
		void SetCharSpaceProportion(float f) { m_charSpaceProportion = f; }
		void SetScreenPos(const math::Vec2& p) { m_pos = p.Xyz(m_pos.z); }
		void SetScreenPos(float x, float y) { m_pos = math::Vec3(x, y, m_pos.z); }
		void SetZ(float z) { m_pos.z = z; }
		void SetLineBreakProportion(float f) { m_lineBreakProportion = f; }

		uint GetCharSize()const { return m_size; }
		float GetCharSpaceProportion()const { return m_charSpaceProportion; }
		float GetLineBreakProportion()const { return m_lineBreakProportion; }

		virtual void Render(const String& text, Font*) = 0;

	protected:
		FontEffect();

		virtual void OnInitialize() = 0;
		virtual void OnShutdown() = 0;

		Handle<Model> GetModel();
		const math::Vec3& GetScreenPos()const { return m_pos; }

	private:
		bool m_initialized;
		Handle<Model> m_hModel;
		math::Vec3 m_pos;
		uint m_size;
		float m_charSpaceProportion;
		float m_lineBreakProportion;
};

/****************************************************************************************
	FontEffect_Color
*****************************************************************************************/

class FontEffect_Color : public FontEffect
{
	public:
		FontEffect_Color() = default;
		virtual ~FontEffect_Color() = default;

		void SetColor(float red, float green, float blue, float alpha = 1.0f);
		void SetColor(const math::Vec4&);
		void Render(const String& text, Font*)override final;

		const math::Vec4& GetColor()const;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		math::Mtx33 GetTextureTransform(const BitmapCharInfo*)const;

		SimplePtr<Effect_Text2d> m_pEffect;
};

