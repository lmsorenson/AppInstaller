#include "zippackage.h"
#include <archive.h>
#include <archive_entry.h>
#include <QDebug>

ZipPackage::ZipPackage(QString path, QString file_name, QString extension) : QObject(nullptr)
{
    path_ = path;
    file_name_ = file_name;
    extension_ = extension;
}

QStringList ZipPackage::list()
{
    qDebug() << "starting list";
    QStringList list;

    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);


    r = archive_read_open_filename(a, path_.toStdString().c_str(), 10240);

    if (r != ARCHIVE_OK)
        return QStringList();

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        list << QString(archive_entry_pathname(entry));
        archive_read_data_skip(a);
    }

    r = archive_read_free(a);
    if (r == ARCHIVE_OK)
        return list;

    return list;
}

static int copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

void ZipPackage::extract()
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_compression_all(a);

    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    QString read_path = path_ + "/" + file_name_ + extension_;
    QString write_directory = path_ + "/" + file_name_ + "/";

    if ((r = archive_read_open_filename(a, read_path.toStdString().c_str(), 10240)))
        exit(1);

    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN)
            exit(1);

        auto new_path = (write_directory + archive_entry_pathname(entry));
        archive_entry_set_pathname(entry, new_path.toStdString().c_str());

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                exit(1);
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            exit(1);

        if (new_path.contains("/Contents/MacOS/", Qt::CaseSensitivity::CaseSensitive) && !new_path.endsWith("/"))
            chmod(new_path.toStdString().c_str(), S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
    }


    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    return;
}