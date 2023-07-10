#ifdef ORMPP_ENABLE_MYSQL
#include "mysql.hpp"
#endif

#ifdef ORMPP_ENABLE_SQLITE3
#include "sqlite.hpp"
#endif

#ifdef ORMPP_ENABLE_PG
#include "postgresql.hpp"
#endif

#include <iostream>

#include "connection_pool.hpp"
#include "dbng.hpp"

//log redirection
#ifdef ORMPP_ENABLE_LOG
class LogHandler : public ormpp::ILogHandler {
public:
	void log(std::string message, ormpp::LogLevel level) override {
		std::cout << message;
	}
};
#endif

using namespace ormpp;
const char *password = "";
const char *ip = "127.0.0.1";
const char *db = "test_ormppdb";

struct person {
  int id;
  std::optional<std::string> name;
  std::optional<int> age;
};
REFLECTION(person, id, name, age)

struct student {
  int id;
  std::string name;
  int age;
};
REFLECTION_WITH_NAME(student, "t_student", id, name, age)

int main() {

#ifdef ORMPP_ENABLE_LOG
	auto logHander = std::make_shared<LogHandler>();
	ormpp::logger::setHandler(logHander.get());
#endif
#ifdef ORMPP_ENABLE_MYSQL
  {
    dbng<mysql> mysql;
    if (mysql.connect(ip, "root", password, db)) {
      mysql.create_datatable<person>(ormpp_auto_key{"id"});
      mysql.delete_records<person>();
      mysql.insert<person>({0, "purecpp"});
      mysql.insert<person>({0, "purecpp", 6});
    }
    else {
      std::cout << "connect fail" << std::endl;
    }
  }

  {
    connection_pool<dbng<mysql>>::instance().init(4, ip, "root", password, db,
                                                  5, 3306);
    auto conn = connection_pool<dbng<mysql>>::instance().get();
    conn_guard guard(conn);
    conn->create_datatable<student>(ormpp_auto_key{"id"});
    auto vec = conn->query<student>();
  }

#endif

#ifdef ORMPP_ENABLE_SQLITE3
  dbng<sqlite> sqlite;
  sqlite.connect(db);
  sqlite.create_datatable<person>(ormpp_auto_key{"id"});
  sqlite.create_datatable<student>(ormpp_auto_key{"id"});

  {
    sqlite.delete_records<person>();
    sqlite.insert<person>({0, "purecpp"});
    sqlite.insert<person>({0, "purecpp", 6});
    auto vec = sqlite.query<person>();
    for (auto &[id, name, age] : vec) {
      std::cout << id << ", " << *name << ", " << *age << "\n";
    }
  }

  {
    sqlite.delete_records<student>();
    sqlite.insert<student>({0, "purecpp", 1});
    sqlite.insert<student>({0, "purecpp", 2});
    sqlite.insert<student>({0, "purecpp", 3});
    sqlite.insert<student>({0, "purecpp", 3});
    {
      auto vec = sqlite.query<student>("name='purecpp'", "order by age desc");
      for (auto &[id, name, age] : vec) {
        std::cout << id << ", " << name << ", " << age << "\n";
      }
    }
    {
      auto vec = sqlite.query<student>("age=3", "order by id desc", "limit 1");
      for (auto &[id, name, age] : vec) {
        std::cout << id << ", " << name << ", " << age << "\n";
      }
    }
  }

#endif

  return 0;
}
