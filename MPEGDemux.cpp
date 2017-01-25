
#include <string>
#include <chrono>
#include "Demultiplexer.hpp"
#include "ProgramMapTable.hpp"
#include "OutputStream.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

std::unique_ptr<OutputStream> create_stream(PID pid, const StreamInfo & stream_info)
{
    std::string file_name;
    switch (stream_info.type())
    {
    case StreamInfo::Type::Audio:
        file_name = "audio_" + std::to_string(pid) + ".raw";
        break;

    case StreamInfo::Type::Video:
        file_name = "video_" + std::to_string(pid) + ".raw";
        break;

    case StreamInfo::Type::Other:
        std::cout << "Stream id " << (int)stream_info.stream_type() << " is unknown\n";
        file_name = "other_" + std::to_string(pid) + ".raw";
        break;
    }
    std::cout << "Creating output stream " << file_name << "\n";
    return std::unique_ptr<OutputStream>(new OutputStream(file_name));
}


int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: demux mpeg-ts-file\n";
    }
    std::ifstream input(argv[1], std::ios::in | std::ios::binary);
    std::vector<uint8_t> read_buffer(32 * 1024 * 1024);

    Demultiplexer demux(create_stream);
    auto start_time = std::chrono::system_clock::now();
    if (!input.good())
    {
        std::cout << "Failed to open: " << argv[1];
        return -1;
    }

    do
    {
        input.read(reinterpret_cast<char *>(read_buffer.data()), read_buffer.size());
        if (input.gcount() > 0)
        {
            demux.on_data(ByteView(&read_buffer[0], (size_t)input.gcount()));
        }
    } while (input.good());

    auto end_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Processed " << demux.packets_processed() << " packets in " << duration << " ms\n";

}



