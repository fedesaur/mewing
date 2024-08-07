#include "../lib/con2db/pgsql.cpp"
#include "../lib/con2redis/src/readreply.cpp"
#include "../lib/con2redis/src/redisfun.cpp"
#include <iostream>

int main() {
    try {
        // Connect to PostgreSQL
        Con2DB db("dbname=ecommerce_db user=federica password=1760539");
        db.create_table();
        db.add_product("Sample Product", 19.99);

        // Connect to Redis
        Con2Redis redis("127.0.0.1", 6379);
        redis.set_value("product_count", "10");
        std::cout << "GET product_count: " << redis.get_value("product_count") << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}



