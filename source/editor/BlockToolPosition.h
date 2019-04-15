//#pragma once
//
//// TODO: use forward declaration instead of #include
//#include "../engine/Pointer.h"
//#include "../engine/Math.h"
//#include "../engine/Model.h"
//#include "../engine/Shader.h"
//#include "BlockPosition.h"
//#include "BlockTool.h"
//
//class BlockToolPosition : public BlockTool
//{
//	public:
//		BlockToolPosition();
//		~BlockToolPosition();
//
//		void Initialize()override;
//		void Update(Block*, const math::CameraTransform&)override;
//		void Render(Block*, const math::CameraTransform&)override;
//
//		bool IsUsing()const override;
//		
//	private:
//		void HandleClick(Block* pBlock, const math::CameraTransform&);
//		void HandleUnclick();
//		void HandleToolUse(Block*, const math::CameraTransform&);
//
//		SimplePtr<Model> m_pModel;
//		SimplePtr<ShaderColorLightDir> m_pShader;
//		SimplePtr<BlockPosition> m_blockPos[3];
//
//		eBlockToolState m_state;
//		BlockPosition* m_pCurrent;
//};
