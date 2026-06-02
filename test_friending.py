import discord
import asyncio
import sys

class TestClient(discord.Client):
    async def on_ready(self):
        print(f"Logged in as {self.user} (ID: {self.user.id})")
        target_id = 612264273636229120 # sivalva
        try:
            user = await self.fetch_user(target_id)
            print(f"Fetched user: {user.name}")
            print("Trying user.send_friend_request(location='Guild Member List')...")
            await user.send_friend_request(location="Guild Member List")
            print("SUCCESS with location context!")
        except Exception as e:
            print(f"FAILED with location context: {type(e).__name__}: {e}")
            if hasattr(e, 'code'):
                print(f"Discord Error Code: {e.code}")
            if hasattr(e, 'status'):
                print(f"HTTP Status: {e.status}")
                
        try:
            user = await self.fetch_user(target_id)
            print("Trying user.send_friend_request()...")
            await user.send_friend_request()
            print("SUCCESS without location context!")
        except Exception as e:
            print(f"FAILED without location: {type(e).__name__}: {e}")
            
        await self.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python test_friending.py <TOKEN>")
        sys.exit(1)
    token = sys.argv[1]
    client = TestClient()
    asyncio.run(client.start(token))
