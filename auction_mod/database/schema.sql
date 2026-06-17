-- BDO Style Auction House Schema for DayZ

CREATE TABLE IF NOT EXISTS items (
    id SERIAL PRIMARY KEY,
    class_name TEXT UNIQUE NOT NULL,
    display_name TEXT NOT NULL,
    base_price BIGINT NOT NULL DEFAULT 1000,
    min_price BIGINT NOT NULL DEFAULT 500,
    max_price BIGINT NOT NULL DEFAULT 2000,
    last_update TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS listings (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    item_id INTEGER REFERENCES items(id) ON DELETE CASCADE,
    seller_id TEXT NOT NULL, -- SteamID64
    price BIGINT NOT NULL,
    quantity INTEGER NOT NULL DEFAULT 1,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS buy_orders (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    item_id INTEGER REFERENCES items(id) ON DELETE CASCADE,
    buyer_id TEXT NOT NULL, -- SteamID64
    price BIGINT NOT NULL,
    quantity INTEGER NOT NULL DEFAULT 1,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS transactions (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    item_id INTEGER REFERENCES items(id) ON DELETE CASCADE,
    seller_id TEXT NOT NULL,
    buyer_id TEXT NOT NULL,
    price BIGINT NOT NULL,
    quantity INTEGER NOT NULL,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS market_stats (
    item_id INTEGER REFERENCES items(id) ON DELETE CASCADE,
    record_date DATE NOT NULL DEFAULT CURRENT_DATE,
    avg_price BIGINT NOT NULL,
    volume INTEGER NOT NULL,
    PRIMARY KEY (item_id, record_date)
);

-- Index for faster lookups
CREATE INDEX IF NOT EXISTS idx_listings_item_price ON listings(item_id, price);
CREATE INDEX IF NOT EXISTS idx_buy_orders_item_price ON buy_orders(item_id, price);

CREATE TABLE IF NOT EXISTS claims (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    player_id TEXT NOT NULL,
    item_class TEXT NOT NULL,
    quantity INTEGER NOT NULL DEFAULT 1,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);
