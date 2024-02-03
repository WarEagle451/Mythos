import os
import pathlib 
import sys
import requests
import time
import urllib

from zipfile import ZipFile

root = f"{pathlib.Path(__file__).parent.parent.parent.resolve()}"
def project_root():
    return root

def download_file(url, filepath):
    filepath = os.path.abspath(filepath)
    os.makedirs(os.path.dirname(filepath), exist_ok = True)
            
    if (type(url) is list):
        for url_option in url:
            print("Downloading", url_option)
            try:
                download_file(url_option, filepath)
                return
            except urllib.error.URLError as e:
                print(f"URL Error encountered: {e.reason}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except urllib.error.HTTPError as e:
                print(f"HTTP Error  encountered: {e.code}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except:
                print(f"Something went wrong. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
        raise ValueError(f"Failed to download {filepath}")
    if not(type(url) is str):
        raise TypeError("Argument 'url' must be of type list or string")

    with open(filepath, 'wb') as f:
        headers = {'User-Agent': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"}
        response = requests.get(url, headers = headers, stream = True)
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.content)
        else:
            downloaded = 0
            total = int(total)
            start_time = time.time()
            for data in response.iter_content(chunk_size = max(int(total / 1000), 1024 * 1024)):
                downloaded += len(data)
                f.write(data)
                
                try:
                    done = int(50 * downloaded / total) if downloaded < total else 50
                    percentage = (downloaded / total) * 100 if downloaded < total else 100
                except ZeroDivisionError:
                    done = 50
                    percentage = 100
                elapsed_time = time.time() - start_time
                try:
                    avg_kb_per_second = (downloaded / 1024) / elapsed_time
                except ZeroDivisionError:
                    avg_kb_per_second = 0.0

                avg_speed_string = '{:.2f} KB/s'.format(avg_kb_per_second)
                if (avg_kb_per_second > 1024):
                    avg_mb_per_second = avg_kb_per_second / 1024
                    avg_speed_string = '{:.2f} MB/s'.format(avg_mb_per_second)
                sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avg_speed_string))
                sys.stdout.flush()
    sys.stdout.write('\n')

def unzip_file(filepath, overwrite = False, delete_zip_file = True):
    zip_path = os.path.abspath(filepath) # get full path of files
    zip_location = os.path.dirname(zip_path)

    zip_content = dict()
    zip_content_size = 0
    with ZipFile(zip_path, 'r') as zip_file_folder:
        for name in zip_file_folder.namelist():
            zip_content[name] = zip_file_folder.getinfo(name).file_size
        zip_content_size = sum(zip_content.values())
        extracted_content_size = 0
        start_time = time.time()
        for zipped_file_name, zipped_file_size in zip_content.items():
            unzipped_filepath = os.path.abspath(f"{zip_location}/{zipped_file_name}")
            os.makedirs(os.path.dirname(unzipped_filepath), exist_ok = True)
            if (overwrite == False and os.path.isfile(unzipped_filepath)):
                zip_content_size -= zipped_file_size
            else:
                zip_file_folder.extract(zipped_file_name, path = zip_location, pwd = None)
                extracted_content_size += zipped_file_size
            try:
                done = int(50 * extracted_content_size / zip_content_size)
                percentage = (extracted_content_size / zip_content_size) * 100
            except ZeroDivisionError:
                done = 50
                percentage = 100
            elapsed_time = time.time() - start_time
            try:
                avg_kb_per_second = (extracted_content_size / 1024) / elapsed_time
            except ZeroDivisionError:
                avg_kb_per_second = 0.0
            avg_speed_string = '{:.2f} KB/s'.format(avg_kb_per_second)
            if (avg_kb_per_second > 1024):
                avg_mb_per_second = avg_kb_per_second / 1024
                avg_speed_string = '{:.2f} MB/s'.format(avg_mb_per_second)
            sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avg_speed_string))
            sys.stdout.flush()
    sys.stdout.write('\n')

    if delete_zip_file:
        os.remove(zip_path) # delete zip file

class version:
    major = 0
    minor = 0
    patch = 0

    def __init__(self, version_major, version_minor, version_patch = 0):
        self.major = version_major
        self.minor = version_minor
        self.patch = version_patch

    @classmethod
    def parse(self, version_string):
        versions = version_string.split('.')
        count = len(versions)
        if (count < 3):
            versions.extend([".0"] * (3 - count))
        return version(int(versions[0]), int(versions[1]), int(versions[2]))

    def __eq__(self, other: object) -> bool:
        return self.major == other.major and self.minor == other.minor and self.patch == other.patch

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)

    def __gt__(self, other: object) -> bool:
        if (self.major > other.major):
            return True
        elif (self.major == other.major):
            if (self.minor > other.minor):
                return True
            elif (self.minor == other.minor):
                return self.patch > other.patch
        return False

    def __lt__(self, other: object) -> bool:
        if (self.major < other.major):
            return True
        elif (self.major == other.major):
            if (self.minor < other.minor):
                return True
            elif (self.minor == other.minor):
                return self.patch < other.patch
        return False

    def __ge__(self, other: object) -> bool:
        return not self.__lt__(other)

    def __le__(self, other: object) -> bool:
        return not self.__gt__(other)