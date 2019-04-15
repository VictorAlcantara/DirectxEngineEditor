#pragma once

#include "StringClass.h"

#define CLASS_TAG(className) public: static const HashString& GetClassTagStatic() { static HashString hash(#className); return hash; } \
	virtual const HashString& GetClassTag()const override { return className::GetClassTagStatic(); }

#define CLASS_TAG_INTERFACE public: virtual const HashString& GetClassTag()const = 0;

enum class eClassRelationship
{
	None,
	Inherits,
	Is,
};

class NonCopyable
{
	protected:
		NonCopyable() = default;
		virtual ~NonCopyable() = default;

	private:
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator = (const NonCopyable&) = delete;
};

template <class T>
class Singleton : public NonCopyable
{
	public:
		static T* GetInstance()
		{
			static T instance;
			return &instance;
		}

		virtual ~Singleton() = default;
};

class ClassStructure
{
	private:
		struct ClassStructureNode
		{
			HashString data;
			ClassStructureNode* pNext;

			ClassStructureNode() : pNext(nullptr) {}
		};

	public:
		ClassStructure()
		{
			m_pNode = nullptr;
		}

		~ClassStructure()
		{
			ClassStructureNode* pCurrent = m_pNode;
			ClassStructureNode* pNext = nullptr;

			while ( pCurrent )
			{
				pNext = pCurrent->pNext;
				delete pCurrent;
				pCurrent = pNext;
			}

			m_pNode = nullptr;
		}

		eClassRelationship GetClassRelationship(const HashString& classTag)const
		{
			for ( auto pNode = m_pNode; pNode; pNode = pNode->pNext )
			{
				if ( pNode->data == classTag )
				{
					if ( !pNode->pNext )
						return eClassRelationship::Is;
					return eClassRelationship::Inherits;
				}
			}

			return eClassRelationship::None;
		}

		template <class TYPE>
		eClassRelationship GetClassRelationship()const
		{
			return GetClassRelationship( TYPE::GetClassTagStatic() );
		}

		bool GetClassRelationship_Is(const HashString& classTag)const
		{
			return GetClassRelationship(classTag) == eClassRelationship::Is;
		}

		bool GetClassRelationship_Has(const HashString& classTag)const
		{
			return GetClassRelationship(classTag) == eClassRelationship::Inherits;
		}

		bool GetClassRelationship_IsOrHas(const HashString& classTag)const
		{
			return GetClassRelationship(classTag) != eClassRelationship::None;
		}

		template <class TYPE>
		bool GetClassRelationship_Is()const
		{
			return GetClassRelationship_Is( TYPE::GetClassTagStatic() );
		}

		template <class TYPE>
		bool GetClassRelationship_Has()const
		{
			return GetClassRelationship_Has( TYPE::GetClassTagStatic() );
		}

		template <class TYPE>
		bool GetClassRelationship_IsOrHas()const
		{
			return GetClassRelationship_IsOrHas( TYPE::GetClassTagStatic() );
		}

		virtual const HashString& GetClassTag()const = 0;

	protected:
		void RegisterInheritance(const HashString& name)
		{
			if ( !m_pNode )
			{
				m_pNode = new ClassStructureNode;
				m_pNode->data = name;
			}
			else
			{
				ClassStructureNode* pLast = m_pNode;

				while ( pLast->pNext )
				{
					pLast = pLast->pNext;
				}

				pLast->pNext = new ClassStructureNode;
				pLast->pNext->data = name;
			}
		}

	private:
		ClassStructureNode* m_pNode;
};

class PublicModule
{
	public:
		PublicModule() : m_initialized(false) {}
		virtual ~PublicModule();

		void Initialize();
		void Shutdown();

		bool IsInitialized()const { return m_initialized; }
		
	protected:
		virtual void OnInitialize() = 0;
		virtual void OnShutdown() = 0;

	private:
		bool m_initialized;
};

class ProtectedModule
{
	public:
		ProtectedModule() : m_initialized(false) {}
		virtual ~ProtectedModule();

		bool IsInitialized()const { return m_initialized; }

	protected:
		void Initialize();
		void Shutdown();

		virtual void OnInitialize() = 0;
		virtual void OnShutdown() = 0;

	private:
		bool m_initialized;
};
