#include "../engine/ScriptReader.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ScriptReader::ScriptReader(const FileReader& file) : ScriptReader()
{
	for ( uint i = 0; i < file.LineCount(); i++ )
	{
		if ( file.GetLine(i).GetSize() > 0 )
			AddLine( String(file.GetLine(i).GetData(), file.GetLine(i).GetSize()) );
	}

	CreateIndex();
}
//-------------------------------------------------------------------
ScriptReader::ScriptReader(const String& script) : ScriptReader()
{
	Array<String> lines;
	string::split( script, '\n', lines );
	for ( uint i = 0; i < lines.Count(); i++ )
	{
		if ( lines[i].Length() > 0 )
			AddLine( lines[i] );
	}

	CreateIndex();
}
//-------------------------------------------------------------------
//	CreateIndex
//-------------------------------------------------------------------
void ScriptReader::CreateIndex()
{
	m_index = new FixedArray<ScriptLine*>( m_lines.Count() );
	uint i = 0;
	for ( auto pNode = m_lines.GetFirst(); pNode; pNode = pNode->GetNext(), i++ )
	{
		m_index->Add( pNode->GetValue().Get() );
	}
}
//-------------------------------------------------------------------
//	AddLine
//-------------------------------------------------------------------
void ScriptReader::AddLine(const String& line)
{
	m_lines.AddLast( new ScriptLine(line) );
}
//-------------------------------------------------------------------
//	GetLineCount
//-------------------------------------------------------------------
uint ScriptReader::GetLineCount()const
{
	return m_lines.Count();
}
//-------------------------------------------------------------------
//	GetLine
//-------------------------------------------------------------------
ScriptLine* ScriptReader::GetLine(uint index)
{
	if ( index >= m_lines.Count() )
	{
		ASSERT( "Index out of bounds" );

		return nullptr;
	}

	return m_index->Get(index);
}
//-------------------------------------------------------------------
const ScriptLine* ScriptReader::GetLine(uint index)const
{
	if ( index >= m_lines.Count() )
	{
		ASSERT( "Index out of bounds" );

		return nullptr;
	}

	return m_index->Get(index);
}
//-------------------------------------------------------------------
//	HasLine
//-------------------------------------------------------------------
bool ScriptReader::HasLine(uint line)const
{
	return line < m_lines.Count();
}
