public static bool BulletTrack = false;
        public static bool Bullet = false;
        internal static string MagicBulletList;
        internal static uint MagicBulletHook;
        internal static byte[] MagicData;
 
public static string magicSearch = "66 0F 7E ?? 38 02 00 00 C7 45 78 40 FE";
        public static byte[] magicByte = new byte[] {
            137, 21, 112, 0, 217, 3, 139, 85, 52, 162, 100, 0, 217, 3, 138, 130, 224, 253, 255, 255, 162, 104, 0,
            217, 3, 138, 130, 225, 253, 255, 255, 162, 108, 0, 217, 3, 160, 100, 0, 217, 3, 129, 61, 104, 0, 217,
            3, 220, 0, 0, 0, 116, 14, 139, 21, 112, 0, 217, 3, 199, 69, 104, 20, 131, 15, 41, 195, 129, 61, 108,
            0, 217, 3, 192, 0, 0, 0, 116, 2, 235, 228, 199, 2, 214, 23, 124, 62, 199, 66, 4, 156, 177, 199, 64, 235, 213 };
 
        public long[] GetMagic(long startAddv = 0x18000000, long endAddv = 0x20000000)
        {
            var tmp = sgn.FindPatternsAllRegion(Patterns.magicSearch, startAddv, endAddv);
            List<long> magicList = new List<long>();
            foreach (var addv in tmp)
            {
                if (Mem.ReadMemory<int>(addv - 4) == 168)
                    magicList.Add(addv - 8);
            }
            return magicList.ToArray();
        }
 
   public static void RestoreMagic()
        {
            if (Settings.MagicBulletList != null && Settings.MagicData != null)
            {
                foreach (var addv in Settings.MagicBulletList)
                {
                    if (Mem.ReadMemory<byte>(addv + 16) == 232)
                    {
                        Mem.WriteMemory(addv + 16, Settings.MagicData);
                    }
                }
            }
        }
 
        public static void InitMagic()
        {
            if (Settings.MagicBulletList != null)
            {
                long addv = Settings.MagicBulletList[0];
                long magicAddv = (long)Mem.AllocateMemory(500);
                Mem.WriteMemory(magicAddv, Patterns.magicByte);
                Mem.WriteMemory<int>(magicAddv + 2, magicAddv + 112);
                Mem.WriteMemory<int>(magicAddv + 10, magicAddv + 100);
                Mem.WriteMemory<int>(magicAddv + 21, magicAddv + 104);
                Mem.WriteMemory<int>(magicAddv + 32, magicAddv + 108);
                Mem.WriteMemory<int>(magicAddv + 37, magicAddv + 100);
                Mem.WriteMemory<int>(magicAddv + 43, magicAddv + 104);
                Mem.WriteMemory<int>(magicAddv + 55, magicAddv + 112);
                Mem.WriteMemory<int>(magicAddv + 69, magicAddv + 108);
                byte[] orginByte = Mem.ReadMemory(addv + 16, 7);
                Mem.WriteMemory(magicAddv + 59, orginByte);
                Settings.MagicData = orginByte;
                Settings.MagicBulletHook = (uint)magicAddv;
            }
            else
            {
                throw new Exception("Magic Bullet List is null !!!");
            }
        }
 
        public static Vector2 GetMagicCoord(Vector3 aim, Vector3 myPos)
        {
            aim.Z += 20;
            float xDif = aim.X - myPos.X, yDif = aim.Y - myPos.Y, zDif = aim.Z - myPos.Z;
            Vector2 coord;
            coord.X = (float)((float)Math.Atan2(xDif, yDif) * 180f / Math.PI);
            coord.Y = (float)((float)Math.Atan2(zDif, Math.Sqrt(xDif * xDif + yDif * yDif)) * 180f / Math.PI);
 
            return coord;
        }
        Vector2 Coordinate;
        public Vector2 GetMagicCoordinate()
        {
 
            Coordinate.X = 0;
            Coordinate.Y = 0;
            Vector3 HeadPos;
            HeadPos.X = 0;
            HeadPos.Y = 0;
            HeadPos.Z = 0;
            Vector2 Screen;
            Screen.X = 0;
            Screen.Y = 0;
            Vector3 Velocity;
            Velocity.X = 0;
            Velocity.Y = 0;
            Velocity.Z = 0;
            float TargetDistance = 9999.0f;
            float CrossDist = 0;
            int Distance4 = 0;
            long viewMatrixAddr = Mem.ReadMemory<uint>(Mem.ReadMemory<uint>(_data.ViewMatrixBase) + 32) + 512;
            D3DMatrix viewMatrix = Algorithms.ReadViewMatrix(viewMatrixAddr);
            foreach (var player in _data.Players)
            {
                if (Algorithms.WorldToScreen3DBox(viewMatrix, new ShpVector3(HeadPos.X, HeadPos.Y - (Settings.bPredict * 1), HeadPos.Z - (Settings.bYAxis * 6)), out ShpVector2 HeadPosition, _window.Width, _window.Height))
                {
                    CrossDist = (float)Math.Sqrt(Math.Pow(Screen.X - _window.Width / 2, 2) + Math.Pow(Screen.Y - _window.Height / 2, 2));
                    if (CrossDist < TargetDistance)
                    {
                        HeadPos = HeadPos;
                        TargetDistance = CrossDist;
                        Velocity = Velocity;
                    }
                    HeadPos.Z += 15;
                    uint tmpAddv = Mem.ReadMemory<uint>(player.Address + tmpOffset);
                    uint BodyAddv = (uint)(tmpAddv + actorOffset);
                    uint BoneAddv = Mem.ReadMemory<uint>(tmpAddv + boneOffset) + 48;
                    ShpVector3 MyPosition = Algorithms.GetBoneWorldPosition(BodyAddv, BoneAddv + 5 * 48);
                    float AddTime = 30 / 10.0f + 1.0f;
                    float Gravity = 5.72f;
                    float xDif = HeadPos.X - MyPosition.X;
                    float yDif = HeadPos.Y - MyPosition.Y;
                    float zDif = HeadPos.Z - MyPosition.Z;
 
                    float Hyp = (float)Math.Sqrt(xDif * xDif + yDif * yDif);
                    Coordinate.X = (float)(Math.Atan2(zDif, Hyp) * 180.0f / 3.1415926535897f);
                    Coordinate.Y = (float)(Math.Atan2(yDif, xDif) * 180.0f / 3.1415926535897f);
 
                }
            }
            return Coordinate;
        }
        public async void MemoryThread()
        {
            bool IsMagicInitialized = false;
            Vector3 MagicCoordinate;
            Vector3 AimRotation;
 
            while (true)
            {
                await Task.Delay(10);
                if (Settings.Bullet)
                {
                    if (IsMagicInitialized == false)
                    {
                        ueSearch.GetMagic();
 
                        InitMagic();
                        Settings.MagicBulletHook = 1;
                        IsMagicInitialized = true;
                    }
 
                    if (GetAsyncKeyState(Keys.LButton))
                    {
                        GetMagicCoordinate();
                        MagicCoordinate.X = Coordinate.X;
                        MagicCoordinate.Y = Coordinate.Y;
                        Mem.WriteMemory<float>(Settings.MagicBulletHook + 44, MagicCoordinate.X);
                        Mem.WriteMemory<float>(Settings.MagicBulletHook + 54, MagicCoordinate.Y);
                    }
 
                    // Refresh magic
                    if (GetAsyncKeyState(Keys.R))
                    {
                        if (IsMagicInitialized == true)
                        {
                            ueSearch.GetMagic();
 
                            Settings.MagicBulletHook = 1;
                            IsMagicInitialized = true;
                        }
                    }
                }
                else if (Settings.Bullet == false && IsMagicInitialized == true)
                {
                    RestoreMagic();
                    Settings.MagicBulletList.TrimEnd();
                    Settings.MagicBulletHook = 0;
                    Settings.Bullet = false;
                }
            }
        }
        public static bool HookMagic(float x, float y)
        {
            if (Settings.MagicBulletList != null && Settings.MagicData != null && Settings.MagicBulletHook != 0)
            {
                foreach (var addv in Settings.MagicBulletList)
                {
                    byte[] tmp = Mem.ReadMemory(addv + 16, 7);
                    if (tmp.SequenceEqual(Settings.MagicData))
                    {
                        HookCall(addv + 16, Settings.MagicBulletHook, new byte[] { 144, 144 });
                    }
                    else if (Mem.ReadMemory<byte>(addv + 16) != 232)
                    {
                        return false;
                    }
                }
                Mem.WriteMemory<float>(Settings.MagicBulletHook + 83, y);
                Mem.WriteMemory<float>(Settings.MagicBulletHook + 90, x);
                return true;
            }
            return false;
        }
 
        public static void HookCall(long hookAddv, long jmpAddv, byte[] add = null)
        {
            byte[] data;
            byte[] tmp = BitConverter.GetBytes((int)(jmpAddv - hookAddv - 5));
            if (add == null)
            {
                data = new byte[5];
                data[0] = 232;
                tmp.CopyTo(data, 1);
            }
            else
            {
                data = new byte[5 + add.Length];
                data[0] = 232;
                tmp.CopyTo(data, 1);
                add.CopyTo(data, 1 + tmp.Length);
            }
            Mem.WriteMemory(hookAddv, data);
        }
