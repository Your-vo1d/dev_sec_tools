#include <QCoreApplication>
#include <QTextStream>

#include "crypto_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream in(stdin);
    QTextStream out(stdout);
    QTextStream err(stderr);

    const QStringList args = app.arguments();

    if (args.size() != 3) {
        out << "Usage:\n"
            << "  " << args.value(0) << " <folder_path> <password>\n";
        return 1;
    }

    const QString folderPath = args.at(1);
    const QString password = args.at(2);

    out << "==== Data Protection System ====\n";
    out << "1 - Encrypt folder\n";
    out << "2 - Decrypt folder\n";
    out << "Select option: ";
    out.flush();

    const QString choice = in.readLine().trimmed();

    bool ok = false;

    if (choice == "1") {
        ok = CryptoManager::instance().encryptFolder(folderPath, password);
    } else if (choice == "2") {
        ok = CryptoManager::instance().decryptFolder(folderPath, password);
    } else {
        err << "Invalid option.\n";
        return 2;
    }

    out << (ok ? "Done.\n" : "Done with errors.\n");
    return ok ? 0 : 3;
}
