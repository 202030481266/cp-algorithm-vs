using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace EditorFontStyles.Tests
{
    /// <summary>Hard memory limit for this process (Windows job object), so a runaway loop cannot exhaust the machine.</summary>
    internal static class MemoryCap
    {
        private const int JobObjectExtendedLimitInformation = 9;
        private const uint JobObjectLimitProcessMemory = 0x100;

        public static void Apply(int megabytes)
        {
            IntPtr job = CreateJobObject(IntPtr.Zero, null);
            var info = new JOBOBJECT_EXTENDED_LIMIT_INFORMATION();
            info.BasicLimitInformation.LimitFlags = JobObjectLimitProcessMemory;
            info.ProcessMemoryLimit = (UIntPtr)((ulong)megabytes << 20);
            int size = Marshal.SizeOf(info);
            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(info, buffer, false);
                if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, buffer, (uint)size)
                    || !AssignProcessToJobObject(job, Process.GetCurrentProcess().Handle))
                {
                    Console.Error.WriteLine("warning: could not apply the memory cap (error " + Marshal.GetLastWin32Error() + ")");
                }
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern IntPtr CreateJobObject(IntPtr attributes, string name);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool SetInformationJobObject(IntPtr job, int infoClass, IntPtr info, uint length);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool AssignProcessToJobObject(IntPtr job, IntPtr process);

        [StructLayout(LayoutKind.Sequential)]
        private struct JOBOBJECT_BASIC_LIMIT_INFORMATION
        {
            public long PerProcessUserTimeLimit;
            public long PerJobUserTimeLimit;
            public uint LimitFlags;
            public UIntPtr MinimumWorkingSetSize;
            public UIntPtr MaximumWorkingSetSize;
            public uint ActiveProcessLimit;
            public UIntPtr Affinity;
            public uint PriorityClass;
            public uint SchedulingClass;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IO_COUNTERS
        {
            public ulong ReadOperationCount;
            public ulong WriteOperationCount;
            public ulong OtherOperationCount;
            public ulong ReadTransferCount;
            public ulong WriteTransferCount;
            public ulong OtherTransferCount;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct JOBOBJECT_EXTENDED_LIMIT_INFORMATION
        {
            public JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
            public IO_COUNTERS IoInfo;
            public UIntPtr ProcessMemoryLimit;
            public UIntPtr JobMemoryLimit;
            public UIntPtr PeakProcessMemoryUsed;
            public UIntPtr PeakJobMemoryUsed;
        }
    }
}
