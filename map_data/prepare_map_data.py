#%%
import shutil
import requests
import os
import geopandas as gpd

MAP_DATA_URL = "https://www.naturalearthdata.com/http//www.naturalearthdata.com/download/110m/physical/ne_110m_land.zip"
HEADERS = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) '
                         'Chrome/58.0.3029.110 Safari/537.3'}


def download_map_data(url, output_path="map_data.zip"):
    r = requests.get(url, headers=HEADERS, stream=True)
    r.raise_for_status()

    with open(output_path, 'wb') as output_file:
        for chunk in r.iter_content(chunk_size=8192):
            output_file.write(chunk)


def unzip_map_data(zip_path, output_path="map_data"):
    shutil.unpack_archive(zip_path, output_path, "zip")


def find_shapefile(path):
    shp_file = None
    for file in os.listdir(path):
        if file.endswith(".shp"):
            shp_file = os.path.join(path, file)
            break
    if shp_file is None:
        raise FileNotFoundError("No shapefile found in map data.")
    return shp_file


def save_polygons_to_file(polygons, output_path="land_polygons.txt"):
    with open(output_path, "w") as output_file:
        for polygon in polygons:
            for point in polygon.exterior.coords:
                output_file.write(f"{point[0]} {point[1]} ")


if __name__ == "__main__":
    print("Downloading map data from Natural Earth Project...")
    download_map_data(MAP_DATA_URL)
    print("Unzipping map data...")
    unzip_map_data("map_data.zip")
    print("Reading map data...")
    shp_file = find_shapefile("map_data")
    map_data = gpd.read_file(shp_file)
    print("Extracting land polygons...")
    land_data = map_data[map_data["featurecla"] == "Land"]
    print(land_data['geometry'])
    print("Save land polygons to file...")
    save_polygons_to_file(land_data['geometry'])

