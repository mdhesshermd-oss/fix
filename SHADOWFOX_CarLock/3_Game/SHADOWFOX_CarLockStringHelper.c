#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

class SHADOWFOX_CarLockStringHelper
{
    private static ref map<int, string> m_DecryptionMap = new map<int, string>;

    static string Decrypt(string hex, int seed)
    {
        string result;
        if (m_DecryptionMap.Find(seed, result)) return result;

        string decrypted = "";
        string charset = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        string hexset = "0123456789ABCDEF";

        for (int i = 0; i < hex.Length(); i += 2)
        {
            seed = (65793 * seed + 8388608) % 4282663;
            if (seed < 0) seed *= -1;

            int byteVal = 0;
            for (int j = 0; j < 2; j++)
            {
                byteVal = (16 * byteVal) + hexset.IndexOf(hex.Get(i + j));
            }
            int charIdx = (byteVal - (seed % 128));
            if (charIdx >= 0 && charIdx < charset.Length())
                decrypted += charset.Get(charIdx);
            else
                decrypted += "?";
        }
        m_DecryptionMap.Insert(seed, decrypted);
        return decrypted;
    }
};

#endif
