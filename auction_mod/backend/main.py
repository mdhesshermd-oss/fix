import os
import datetime
import uuid
from typing import List, Optional
from fastapi import FastAPI, Depends, HTTPException, Header
from sqlalchemy import create_engine, Column, Integer, String, BigInteger, TIMESTAMP, ForeignKey, Date, func
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session
from pydantic import BaseModel
from fastapi_utils.tasks import repeat_every

# --- Configuration ---
DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://user:pass@localhost/dayz_auction")
API_KEY = os.getenv("AUCTION_API_KEY", "super-secret-key")

# --- Database Setup ---
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class Item(Base):
    __tablename__ = "items"
    id = Column(Integer, primary_key=True, index=True)
    class_name = Column(String, unique=True, index=True)
    display_name = Column(String)
    base_price = Column(BigInteger, default=1000)
    min_price = Column(BigInteger, default=500)
    max_price = Column(BigInteger, default=2000)

class Listing(Base):
    __tablename__ = "listings"
    id = Column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    item_id = Column(Integer, ForeignKey("items.id"))
    seller_id = Column(String)
    price = Column(BigInteger)
    quantity = Column(Integer, default=1)
    created_at = Column(TIMESTAMP, default=datetime.datetime.utcnow)

class BuyOrder(Base):
    __tablename__ = "buy_orders"
    id = Column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    item_id = Column(Integer, ForeignKey("items.id"))
    buyer_id = Column(String)
    price = Column(BigInteger)
    quantity = Column(Integer, default=1)
    created_at = Column(TIMESTAMP, default=datetime.datetime.utcnow)

class Claim(Base):
    __tablename__ = "claims"
    id = Column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    player_id = Column(String)
    item_class = Column(String)
    quantity = Column(Integer, default=1)
    created_at = Column(TIMESTAMP, default=datetime.datetime.utcnow)

class Transaction(Base):
    __tablename__ = "transactions"
    id = Column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    item_id = Column(Integer, ForeignKey("items.id"))
    seller_id = Column(String)
    buyer_id = Column(String)
    price = Column(BigInteger)
    quantity = Column(Integer)
    timestamp = Column(TIMESTAMP, default=datetime.datetime.utcnow)

# --- Pricing Logic ---
def update_market_prices(db: Session):
    items = db.query(Item).all()
    for item in items:
        sell_count = db.query(func.sum(Listing.quantity)).filter(Listing.item_id == item.id).scalar() or 0
        buy_count = db.query(func.sum(BuyOrder.quantity)).filter(BuyOrder.item_id == item.id).scalar() or 0

        if buy_count > sell_count * 2 and buy_count > 0:
            item.max_price = int(item.max_price * 1.05)
            item.base_price = int(item.base_price * 1.02)
        elif sell_count > buy_count * 2 and sell_count > 0:
            item.min_price = int(item.min_price * 0.95)
            item.base_price = int(item.base_price * 0.98)

        item.min_price = max(1, item.min_price)
        item.base_price = max(item.min_price + 1, item.base_price)
        item.max_price = max(item.base_price + 1, item.max_price)
    db.commit()

# --- Schemas ---
class OrderCreate(BaseModel):
    item_class: str
    player_id: str
    price: int
    quantity: int

# --- Middleware ---
def verify_api_key(x_api_key: str = Header(...)):
    if x_api_key != API_KEY:
        raise HTTPException(status_code=403, detail="Invalid API Key")

# --- API Endpoints ---
app = FastAPI()

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.on_event("startup")
@repeat_every(seconds=3600)
def hourly_price_update():
    db = SessionLocal()
    try:
        update_market_prices(db)
    finally:
        db.close()

@app.get("/market/items", dependencies=[Depends(verify_api_key)])
def get_items(db: Session = Depends(get_db)):
    items = db.query(Item).all()
    result = []
    for item in items:
        sell_count = db.query(func.sum(Listing.quantity)).filter(Listing.item_id == item.id).scalar() or 0
        buy_count = db.query(func.sum(BuyOrder.quantity)).filter(BuyOrder.item_id == item.id).scalar() or 0
        result.append({
            "class_name": item.class_name,
            "display_name": item.display_name,
            "base_price": item.base_price,
            "min_price": item.min_price,
            "max_price": item.max_price,
            "stock": sell_count,
            "demand": buy_count
        })
    return result

@app.post("/market/sell", dependencies=[Depends(verify_api_key)])
def place_sell_order(order: OrderCreate, db: Session = Depends(get_db)):
    item = db.query(Item).filter(Item.class_name == order.item_class).first()
    if not item:
        item = Item(class_name=order.item_class, display_name=order.item_class)
        db.add(item)
        db.commit()
        db.refresh(item)

    matching_buy = db.query(BuyOrder).filter(
        BuyOrder.item_id == item.id,
        BuyOrder.price >= order.price
    ).order_by(BuyOrder.price.desc(), BuyOrder.created_at.asc()).first()

    if matching_buy:
        # Create claim for buyer
        claim = Claim(player_id=matching_buy.buyer_id, item_class=order.item_class, quantity=1)
        db.add(claim)

        transaction = Transaction(
            item_id=item.id,
            seller_id=order.player_id,
            buyer_id=matching_buy.buyer_id,
            price=matching_buy.price,
            quantity=1
        )
        db.add(transaction)

        if matching_buy.quantity > 1:
            matching_buy.quantity -= 1
        else:
            db.delete(matching_buy)
        db.commit()
        return {"status": "sold", "price": matching_buy.price, "buyer_id": matching_buy.buyer_id}

    new_listing = Listing(
        item_id=item.id,
        seller_id=order.player_id,
        price=order.price,
        quantity=order.quantity
    )
    db.add(new_listing)
    db.commit()
    return {"status": "listed"}

@app.post("/market/buy", dependencies=[Depends(verify_api_key)])
def place_buy_order(order: OrderCreate, db: Session = Depends(get_db)):
    item = db.query(Item).filter(Item.class_name == order.item_class).first()
    if not item:
        raise HTTPException(status_code=404, detail="Item not found")

    matching_sell = db.query(Listing).filter(
        Listing.item_id == item.id,
        Listing.price <= order.price
    ).order_by(Listing.price.asc(), Listing.created_at.asc()).first()

    if matching_sell:
        # Create claim for buyer (the person who just sent the buy order)
        claim = Claim(player_id=order.player_id, item_class=item.class_name, quantity=1)
        db.add(claim)

        transaction = Transaction(
            item_id=item.id,
            seller_id=matching_sell.seller_id,
            buyer_id=order.player_id,
            price=matching_sell.price,
            quantity=1
        )
        db.add(transaction)

        if matching_sell.quantity > 1:
            matching_sell.quantity -= 1
        else:
            db.delete(matching_sell)
        db.commit()
        return {"status": "bought", "price": matching_sell.price, "seller_id": matching_sell.seller_id}

    new_buy_order = BuyOrder(
        item_id=item.id,
        buyer_id=order.player_id,
        price=order.price,
        quantity=order.quantity
    )
    db.add(new_buy_order)
    db.commit()
    return {"status": "ordered"}

@app.get("/market/claims/{player_id}", dependencies=[Depends(verify_api_key)])
def get_claims(player_id: str, db: Session = Depends(get_db)):
    return db.query(Claim).filter(Claim.player_id == player_id).all()

@app.post("/market/claims/collect/{claim_id}", dependencies=[Depends(verify_api_key)])
def collect_claim(claim_id: str, db: Session = Depends(get_db)):
    claim = db.query(Claim).filter(Claim.id == claim_id).first()
    if not claim:
        raise HTTPException(status_code=404, detail="Claim not found")
    db.delete(claim)
    db.commit()
    return {"status": "collected"}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
