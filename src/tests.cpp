#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>

#include "ConsoleLogger.h"
#include "RecursivePathStepper.h"
#include "Aes256Algorithm.h"
#include "CryptoManager.h"

class CryptoTestSuite : public QObject {
    Q_OBJECT

private:
    QTemporaryDir testDir;
    ConsoleLogger testLogger;
    Aes256Algorithm testAlgo;
    RecursivePathStepper testStepper;

    bool createTestFile(const QString& name, const QByteArray& data, QString& outPath) {
        outPath = testDir.path() + QDir::separator() + name;
        QFile f(outPath);
        if (!f.open(QIODevice::WriteOnly)) return false;
        f.write(data);
        f.close();
        return true;
    }

private slots:
    void initTestCase() {
        QVERIFY(testDir.isValid());
    }

    void cleanup() {
#ifdef UNIT_TESTS
        CryptoManager::resetForTesting();
#endif
    }

    void testConsoleLogger_Info() {
        testLogger.info("TestMessage");
        testLogger.info("");
    }

    void testConsoleLogger_Error() {
        testLogger.error("TestError");
        testLogger.error("Ошибка: UTF-8");
    }

    void testStepper_CollectFiles() {
        auto empty = testStepper.collectFiles("/tmp/nonexistent_path_test_123");
        QVERIFY(empty.empty());

        QString rootFile;
        QVERIFY(createTestFile("root.txt", "data", rootFile));

        QDir dir(testDir.path());
        dir.mkpath("sub/deep");

        QString f1, f2, f3;
        QVERIFY(createTestFile("sub/a.dat", "data", f1));
        QVERIFY(createTestFile("sub/deep/b.log", "data", f2));
        QVERIFY(createTestFile("sub/c.jpg", "data", f3));

        auto files = testStepper.collectFiles(testDir.path());
        QCOMPARE(files.size(), 4);
        for (const auto& f : files) {
            QVERIFY(QFile::exists(f));
        }
    }

    void testAlgorithm_Name() {
        QCOMPARE(testAlgo.name(), QString("AES-256-CBC"));
    }

    void testAlgorithm_IsEncrypted() {
        QString plain;
        QVERIFY(createTestFile("plain.txt", "Hello", plain));
        QVERIFY(!testAlgo.isEncryptedFile(plain));

        QString tiny;
        QVERIFY(createTestFile("tiny.bin", QByteArray::fromRawData("ab", 2), tiny));
        QVERIFY(!testAlgo.isEncryptedFile(tiny));
    }

    void testAlgorithm_Roundtrip() {
        QByteArray original = "SecretData_1234567890_!@#$%^&*()";
        QString path;
        QVERIFY(createTestFile("round.txt", original, path));

        QVERIFY(testAlgo.encrypt(path, "correct_pass"));
        QVERIFY(testAlgo.isEncryptedFile(path));
        QVERIFY(testAlgo.decrypt(path, "correct_pass"));

        QFile f(path);
        f.open(QIODevice::ReadOnly);
        QCOMPARE(f.readAll(), original);
    }

    void testAlgorithm_ReEncryptionProtection() {
        QString path;
        QVERIFY(createTestFile("reenc.txt", "data", path));
        QVERIFY(testAlgo.encrypt(path, "pass"));
        QVERIFY(!testAlgo.encrypt(path, "pass"));
        QVERIFY(testAlgo.isEncryptedFile(path));
    }

    void testAlgorithm_WrongKey() {
        QString path;
        QVERIFY(createTestFile("wrongkey.txt", "protected", path));
        testAlgo.encrypt(path, "right_pass");

        QVERIFY(!testAlgo.decrypt(path, "wrong_pass"));
        QVERIFY(!testAlgo.decrypt(path, "not_encrypted"));
    }

    void testAlgorithm_EmptyFile() {
        QString path;
        QVERIFY(createTestFile("empty.txt", QByteArray(), path));
        QVERIFY(testAlgo.encrypt(path, "pass"));
        QVERIFY(testAlgo.decrypt(path, "pass"));
        QFile f(path);
        f.open(QIODevice::ReadOnly);
        QCOMPARE(f.size(), qint64(0));
    }

    void testManager_Singleton() {
        CryptoManager& m1 = CryptoManager::instance();
        CryptoManager& m2 = CryptoManager::instance();
        QCOMPARE(&m1, &m2);
    }

    void testManager_EncryptDecryptFlow() {
        CryptoManager::instance().init(&testLogger, &testAlgo);

        QString path;
        QVERIFY(createTestFile("mgr.txt", "manager_test", path));
        QVERIFY(CryptoManager::instance().encryptFile(path, "mgr_pass"));
        QVERIFY(CryptoManager::instance().decryptFile(path, "mgr_pass"));
    }

    void testManager_NonExistentPath() {
        CryptoManager::instance().init(&testLogger, &testAlgo);
        QVERIFY(!CryptoManager::instance().encryptFile("/tmp/missing_123.txt", "pass"));
        QVERIFY(!CryptoManager::instance().decryptFile("/tmp/missing_123.txt", "pass"));
    }

    void testManager_DoubleEncryptionBlock() {
        CryptoManager::instance().init(&testLogger, &testAlgo);
        QString path;
        QVERIFY(createTestFile("mgr_block.txt", "block", path));
        testAlgo.encrypt(path, "pass");
        QVERIFY(!CryptoManager::instance().encryptFile(path, "pass"));
    }
};

QTEST_GUILESS_MAIN(CryptoTestSuite)
#include "tests.moc"
