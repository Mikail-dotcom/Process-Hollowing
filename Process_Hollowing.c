#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

int main() {
    // 1. Kullanıcıdan hedef sürecin (örneğin notepad.exe) PID'sini alıyoruz
    // PID: Process ID → her çalışan sürecin benzersiz numarasıdır
    DWORD pid;
    printf("Notepad PID gir: ");
    scanf("%lu", &pid);

    // 2. Hedef süreci açıyoruz
    // PROCESS_ALL_ACCESS: süreci tam yetkiyle açmak istiyoruz
    // FALSE: handle miras alınmasın (child process görmesin)
    // pid: hedef süreç ID'si
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        // Eğer NULL dönerse işlem açılamamış demektir
        // GetLastError() → Windows hata kodunu döner
        printf("OpenProcess hatası! Kod: %d\n", GetLastError());
        return 1;
    }

    // 3. Hedef sürecin içinde bellek ayırıyoruz
    // NULL: Windows kendisi adres seçsin
    // 1024: 1 KB bellek ayır
    // MEM_COMMIT | MEM_RESERVE: belleği rezerve et ve commit et
    // PAGE_READWRITE: okuma ve yazmaya izin ver
    LPVOID remoteBuffer = VirtualAllocEx(hProcess, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
        printf("VirtualAllocEx hatası! Kod: %d\n", GetLastError());
        return 1;
    }

    // 4. Yazmak istediğimiz mesaj
    // Burada payload yerine sadece bir string kullanıyoruz (basit test için)
    char msg[] = "Hello from injection!";
    SIZE_T written;

    // 5. Mesajı hedef sürecin belleğine yazıyoruz
    // remoteBuffer: az önce ayırdığımız adres
    // msg: yazmak istediğimiz data
    // sizeof(msg): boyutu
    // &written: kaç byte yazıldığını döndürür
    if (!WriteProcessMemory(hProcess, remoteBuffer, msg, sizeof(msg), &written)) {
        printf("WriteProcessMemory hatası! Kod: %d\n", GetLastError());
        return 1;
    }

    printf("Belleğe yazıldı! Adres: %p\n", remoteBuffer);

    // 6. İşimiz bitti → handle'ı kapatıyoruz
    CloseHandle(hProcess);
    return 0;
}
