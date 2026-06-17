# DayZ BDO-Style Auction House Mod

This mod implements a Central Market (Auction House) for DayZ, inspired by Black Desert Online's mechanics. It includes dynamic pricing, buy/sell order matching, and a PostgreSQL backend.

## Features
- **BDO Pricing:** Prices fluctuate every hour based on supply and demand.
- **Buy Orders (Pre-orders):** Place orders for items not yet in stock.
- **PostgreSQL Backend:** High-performance, persistent data storage.
- **Expansion Integration:** Uses DayZ Expansion currency.

## Components
1. **Mod Code:** Enforce Script for DayZ (4_World, 5_Mission).
2. **Backend:** Python FastAPI REST service.
3. **Database:** PostgreSQL schema.

## Installation

### 1. Database Setup
- Install PostgreSQL.
- Create a database (e.g., `dayz_auction`).
- Run the SQL commands in `database/schema.sql`.

### 2. Backend Setup
- Navigate to the `backend` folder.
- Install dependencies: `pip install -r requirements.txt`.
- Set environment variables:
  - `DATABASE_URL=postgresql://user:password@localhost/dayz_auction`
  - `AUCTION_API_KEY=your-secure-key`
- Run the server: `python main.py`.

### 3. Mod Installation
- Add the mod files to your DayZ Server.
- Ensure **DayZ Expansion** and an **RPC Manager** (like JM-CF) are installed.
- Configure the mod in `$profile:AuctionHouse/config.json` with your Backend URL and API Key.

## Usage
- Open the Auction House menu in-game.
- Browse items, check stock and current market prices.
- Place sell orders for items in your hands.
- Place buy orders for items you need.
- Claim purchased items via the "Check Claims" button (spawns items at your feet).
