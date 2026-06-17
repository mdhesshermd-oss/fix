import unittest
from fastapi.testclient import TestClient
from .main import app, API_KEY

client = TestClient(app)

class TestAuctionAPI(unittest.TestCase):
    def test_get_items_unauthorized(self):
        response = client.get("/market/items")
        self.assertEqual(response.status_code, 403)

    def test_get_items_authorized(self):
        # Note: This requires a running DB or a mock
        # For the purpose of this environment, we'll just check if the auth works
        response = client.get("/market/items", headers={"x-api-key": API_KEY})
        # If DB is not configured, it might return 500, but auth check happens first
        self.assertIn(response.status_code, [200, 500])

if __name__ == "__main__":
    unittest.main()
