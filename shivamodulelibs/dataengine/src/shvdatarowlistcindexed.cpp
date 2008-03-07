#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlistc_indexed.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../include/shvdataengine.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListC_Indexed::SHVDataRowListC_Indexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index): SHVDataRowListC_SQLite(session, dataStruct)
{
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
	SortIndex = index;
	SHVASSERT(GetStruct()->GetIndex(index));
	if (!GetStruct()->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
SHVStringUTF8 query = BuildQuery(condition, false);
	if (Ok)
		Statement = SQLite->PrepareUTF8(Ok, query, rest);
	Eof = !Ok;
}


/*************************************
 * Destructor
 *************************************/
SHVDataRowListC_Indexed::~SHVDataRowListC_Indexed()
{
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool ok;
SHVSQLiteStatementRef statement;
	sql.Format("drop table memdb.%s", IndexTableName.GetSafeBuffer());
	Statement = NULL;
	statement = SQLite->ExecuteUTF8(ok, sql, rest);
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListC_Indexed::GetRowCount() const
{
	return RowCount;
}

/*************************************
 * BuildQuery
 *************************************/
SHVStringBufferUTF8 SHVDataRowListC_Indexed::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVStringUTF8 col;
SHVStringUTF8 colDefs;
SHVStringUTF8 cols;
SHVStringUTF8 joinCond;
SHVStringUTF8 type;
SHVStringSQLite rest("");
SHVString8 condition8;
SHVString8 orderby8;
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
const SHVDataStructC& Struct = *GetStruct();
const SHVDataRowKey& Key = *Struct.GetIndex(0);
bool first = true;
SHVSQLiteStatementRef statement;
long rc;

// Let the magic begin
	IndexTableName.Format("%s%d", Struct.GetTableName().GetSafeBuffer(), GetDataSession()->GetFactory()->GetDataEngine().GetModuleList().CreateTag());
	for (size_t i = 0; i < Key.Count(); i++)
	{
	size_t colIdx;
		if (Struct.FindColumnIndex(colIdx, Key[i].Key))
		{
			if (!first)
			{
				first = false;
				colDefs += ", ";
				cols += ", ";
				joinCond += " and ";
			}
			switch (Struct[i]->GetDataType())
			{
				case SHVDataVariant::TypeInt:
					type = __SQLITE_TYPE_INT;
					break;
				case SHVDataVariant::TypeBool:
					type = __SQLITE_TYPE_BOOL;
					break;
				case SHVDataVariant::TypeDouble:
					type = __SQLITE_TYPE_DOUBLE;
					break;
				case SHVDataVariant::TypeString:
					type.Format("%s(%d)", __SQLITE_TYPE_STRING, Struct[i]->GetDataLength());
					break;
				case SHVDataVariant::TypeTime:
					type = __SQLITE_TYPE_DATETIME;
			}
			col.Format("%s %s", Struct[i]->GetColumnName().GetSafeBuffer(), type.GetSafeBuffer());
			colDefs += col;
			col.Format("memdb.%s.%s = %s.%s", 
				IndexTableName.GetSafeBuffer(),
				Struct[i]->GetColumnName().GetSafeBuffer(),
				Struct.GetTableName().GetSafeBuffer(),
				Struct[i]->GetColumnName().GetSafeBuffer());
			joinCond += col;
			cols += Struct[i]->GetColumnName().GetSafeBuffer();
		}
		queryUTF8.Format("create table memdb.%s(idx integer primary key autoincrement, %s)", 
			IndexTableName.GetSafeBuffer(), 
			cols.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			Ok = SHVBool::True;
		GetDataSession()->GetFactory()->GetDataEngine().BuildKeySQL(Struct.GetIndex(SortIndex), condition8, orderby8, Struct.GetTableName().GetSafeBuffer(), reverse);
		if (Ok)
		{
			queryUTF8.Format("insert into memdb.%s(%s) select %s from %s %s %s order by %s", 
				IndexTableName.GetSafeBuffer(), cols.GetSafeBuffer(), 
				cols.GetSafeBuffer(), 
				Struct.GetTableName(),
				(condition.IsNull() || condition == _T("") ? "" : "where"),
				condition.ToStrUTF8().GetSafeBuffer(),
				orderby8.GetSafeBuffer()
			);
			statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
			if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
				Ok = SHVBool::True;
			queryUTF8.Format("select count(*) from memdb.%s", IndexTableName.GetSafeBuffer());
			statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
			if (Ok.GetError() == SHVSQLiteWrapper::SQLite_ROW)
				Ok = SHVBool::True;			
			statement->GetLong(rc, 0);
			RowCount = rc;
		}
		if (Ok)
		{
			queryUTF8.Format("select %s.* from %s join memdb.%s on %s where %s order by idx",
				Struct.GetTableName().GetSafeBuffer(),
				Struct.GetTableName().GetSafeBuffer(),
				IndexTableName.GetSafeBuffer(),
				joinCond.GetSafeBuffer(),
				condition8.GetSafeBuffer());
		}		
	}
	return queryUTF8.ReleaseBuffer();	
}

/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowListC_Indexed::AdjustRowCount(int delta)
{
	RowCount += delta;
}
