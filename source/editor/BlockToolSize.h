//#pragma once
//
//#include "../engine/Math.h"
//#include "Block.h"
//#include "BlockResize.h"
//#include "../engine/Model.h"
//#include "../engine/Shader.h"
//#include "BlockTool.h"
//
//class BlockToolSize : public BlockTool
//{
//	public:
//		BlockToolSize();
//		~BlockToolSize();
//
//		void Initialize()override;
//		void Update(Block*, const math::CameraTransform&)override;
//		void Render(Block*, const math::CameraTransform&)override;
//
//		bool IsUsing()const override;
//
//	private:
//		void HandleClick(Block*, const math::CameraTransform&);
//		void HandleUnclick();
//		void HandleToolUse(Block*);
//
//		bool m_using;
//		SimplePtr<BlockResize> m_resizes[12];
//		BlockResize* m_pCurrent;
//};
