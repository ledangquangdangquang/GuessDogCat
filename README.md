<h1 align = "center">Push Button AI: Dog or Cat?</h1>

# Feature
- Ai read image.
- Save in **PostgresSQL server**
# Create Database
```sql
-- 1. Tạo database
CREATE DATABASE guessdogcat_database;

-- 2. Chuyển sang database mới
\c guessdogcat_database

-- 3. Tạo schema
CREATE SCHEMA IF NOT EXISTS guessdogcat_schema;

-- 4. Tạo bảng history trong schema
CREATE TABLE IF NOT EXISTS guessdogcat_schema.history (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    image_name VARCHAR(100),
    user_choice VARCHAR(50),
    ai_answer VARCHAR(50)
);

```
# Install 
Build it in qt.
# SCP images
```
scp -r ./images pi@192.168.30.43:/usr/local/bin
```
