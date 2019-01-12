#include "../include/xdb/sqlite3"

#include "../include/xcept"
#include <cassert>
#include <gsl/gsl>

#define SQLITE3_EXEC(_Cmd) \
	if(int __error = (_Cmd); __error != SQLITE_OK) \
		throw xcept::io_error(sqlite3_errstr(__error));

#define SQLITE3_EXEC_STMT(_Obj, _Cmd) \
	if(int __error = (_Cmd); __error != SQLITE_OK) \
		throw xcept::io_error(sqlite3_errmsg(sqlite3_db_handle(_Obj.get())));

xdb::sqlite3::statement::statement(::sqlite3_stmt * stmt) :
	ptr(stmt)
{
	assert(stmt != nullptr);

	for(size_t i = 0; i < column_count(); i++)
		columns[get_column(i).name] = i;
}

void xdb::sqlite3::statement::reset()
{
	SQLITE3_EXEC(sqlite3_reset(ptr.get()));
}

bool xdb::sqlite3::statement::is_busy() const
{
	return (sqlite3_stmt_busy(const_cast<sqlite3_stmt*>(ptr.get())) != 0);
}

bool xdb::sqlite3::statement::step()
{
	int error = sqlite3_step(ptr.get());
	if(error == SQLITE_OK)
		return false;
	if(error == SQLITE_DONE)
		return false;
	if(error == SQLITE_ROW)
		return true;
	SQLITE3_EXEC(error);
	assert(false);
}

size_t xdb::sqlite3::statement::column_count() const
{
	return gsl::narrow<size_t>(sqlite3_column_count(const_cast<sqlite3_stmt*>(ptr.get())));
}

xdb::sqlite3::statement::column const xdb::sqlite3::statement::get_column(size_t index) const
{
	return column(ptr.get(), index);
}

xdb::sqlite3::statement::column const xdb::sqlite3::statement::get_column(std::string const & name) const
{
	return get_column(columns.at(name));
}




size_t xdb::sqlite3::statement::param_count() const
{
	return gsl::narrow<size_t>(sqlite3_bind_parameter_count(const_cast<sqlite3_stmt*>(ptr.get())));
}

char const * xdb::sqlite3::statement::param_name(size_t index) const
{
	return sqlite3_bind_parameter_name(const_cast<sqlite3_stmt*>(ptr.get()), gsl::narrow<int>(index));
}

size_t xdb::sqlite3::statement::param_index(std::string const & param) const
{
	auto const index = sqlite3_bind_parameter_index(const_cast<sqlite3_stmt*>(ptr.get()), param.c_str());
	if(index <= 0)
		throw xcept::io_error("param " + param + " not found!");
	return gsl::narrow<size_t>(index);
}

xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, int32_t value)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_int(ptr.get(), gsl::narrow<int>(param), value));
	return *this;
}

xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, int64_t value)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_int64(ptr.get(), gsl::narrow<int>(param), value));
	return *this;
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, double value)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_double(ptr.get(), gsl::narrow<int>(param), value));
	return *this;
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, std::string const & value)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_text(
		ptr.get(),
		gsl::narrow<int>(param),
		value.c_str(),
		gsl::narrow<int>(value.size()),
		SQLITE_TRANSIENT));
	return *this;
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, dbnull_t)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_null(ptr.get(), gsl::narrow<int>(param)));
	return *this;
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(size_t param, void const * data, size_t length)
{
	SQLITE3_EXEC_STMT(ptr, sqlite3_bind_blob(
		ptr.get(),
		gsl::narrow<int>(param),
		data,
		gsl::narrow<int>(length),
		SQLITE_TRANSIENT));
	return *this;
}

xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, int32_t value)
{
	return bind(param_index(param), value);
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, int64_t value)
{
	return bind(param_index(param), value);
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, double value)
{
	return bind(param_index(param), value);
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, std::string const & value)
{
	return bind(param_index(param), value);
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, dbnull_t)
{
	return bind(param_index(param), null);
}
xdb::sqlite3::statement & xdb::sqlite3::statement::bind(std::string const & param, void const * data, size_t length)
{
	return bind(param_index(param), data, length);
}




xdb::sqlite3::statement::column::column(::sqlite3_stmt const * _stmt, size_t idx) :
  stmt(_stmt),
  index(idx),
  name(sqlite3_column_name(const_cast<sqlite3_stmt*>(_stmt), gsl::narrow<int>(index)))
{

}

int xdb::sqlite3::statement::column::type() const
{
	return sqlite3_column_type(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index));
}

size_t xdb::sqlite3::statement::column::size() const
{
	return gsl::narrow<size_t>(sqlite3_column_bytes(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index)));
}

int32_t xdb::sqlite3::statement::column::as_int32() const
{
	return gsl::narrow<int32_t>(sqlite3_column_int(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index)));
}

int64_t xdb::sqlite3::statement::column::as_int64() const
{
	return gsl::narrow<int64_t>(sqlite3_column_int64(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index)));
}

double xdb::sqlite3::statement::column::as_double() const
{
	return sqlite3_column_double(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index));
}

std::string xdb::sqlite3::statement::column::as_text() const
{
	return std::string(
	  reinterpret_cast<char const *>(sqlite3_column_text(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index))),
		this->size());
}

std::vector<std::byte> xdb::sqlite3::statement::column::as_blob() const
{
	std::vector<std::byte> blob(size());
	memcpy(blob.data(), sqlite3_column_blob(const_cast<sqlite3_stmt*>(stmt), gsl::narrow<int>(index)), blob.size());
	return blob;
}














xdb::sqlite3::db::db(char const * fileName) :
	ptr()
{
	::sqlite3 * db_ptr = nullptr;
	SQLITE3_EXEC(sqlite3_open(fileName, &db_ptr));
	assert(db_ptr != nullptr);
	ptr.reset(db_ptr);
}

xdb::sqlite3::statement xdb::sqlite3::db::prepare(std::string const & sql)
{
	sqlite3_stmt *stmt;
	SQLITE3_EXEC(sqlite3_prepare_v2(ptr.get(), sql.c_str(), gsl::narrow<int>(sql.size()), &stmt, nullptr));
	assert(stmt != nullptr);
	return statement(stmt);
}
