//#pragma once
//
//#include "../engine/Entity.h"
//#include "../engine/Model.h"
//#include "../engine/Light.h"
//#include "../engine/Gradient.h"
//#include "../engine/CollisionInfo.h"
//
//class Entity_Rocket : public Entity
//{
//	CLASS_TAG( Entity_Rocket );
//
//	public:
//		Entity_Rocket();
//		virtual ~Entity_Rocket();
//
//		void Launch(Handle<Entity> hOwner, const math::Vec3& pos, const math::Vec3&dir);
//
//		Handle<Entity> GetOwner();
//
//	protected:
//		void Update()override final;
//		void Initialize()override final;
//		void Shutdown()override final;
//
//		void OnPhysicsUpdate(Contact*, uint contactCount)override final;
//
//	private:
//		void CreateModel();
//
//		Handle<Entity> m_hOwner;
//		Handle<Model> m_hModel;
//		Handle<PointLight> m_hPointLight;
//		Gradient<float, GradientTransition_Smooth, GradientPeriod_Invert> m_lightGradient;
//
//		//CollisionInfo_Sphere* m_pCollision;
//};
