#ifndef CAPTAL_ENCODING_HPP_INCLUDED
#define CAPTAL_ENCODING_HPP_INCLUDED

#include <string>
#include <array>

namespace cpt
{

/*
struct some_encoding
{
    using char_type = xxx; //Replace "xxx" by the integer type used to represent the encoding.

    template<typename InputIt, typename T>
    static constexpr InputIt decode(InputIt begin, InputIt end, T& out) noexcept
    {
        //Convert the first character of the sequence [begin, end[ to the UTF-32 codepoint.
        //Write it in "out" then return the iterator pointing on the next character of the decoded sequence.

        return begin;
    }

    template<typename OutputIt, typename T>
    static constexpr OutputIt encode(T code, OutputIt out) noexcept
    {
        //Convert the UTF-32 codepoint to the sequence representing the character in the encoding.
        //Write the sequence in "out" then return the iterator pointing on the next character of the output sequence.

        return out;
    }

    template<typename InputIt>
    static constexpr InputIt next(InputIt begin, InputIt end) noexcept
    {
        //Advance "begin" to the next character in the sequence [begin, end[.
        //Return the iterator pointing on the next character of the sequence.

        return begin;
    }

    template<typename InputIt>
    static constexpr std::size_t count(InputIt begin, InputIt end) noexcept
    {
        //Return the count of "real" characters in the sequence [begin, end[. (For multi-bytes encoding)

        std::size_t count{};
        //...
        return count;
    }

    static constexpr std::size_t max_char_length() noexcept
    {
        //Return the highest count of values to represent a single character.
        //e.g. UTF-8 encoding can use 6 values to represent one character.

        return x;
    }
};
*/

namespace impl
{

inline constexpr std::array<char32_t, 666> lowers
{
    0x0061u, 0x0062u, 0x0063u, 0x0064u, 0x0065u, 0x0066u, 0x0067u, 0x0068u, 0x0069u,
    0x006Au, 0x006Bu, 0x006Cu, 0x006Du, 0x006Eu, 0x006Fu, 0x0070u, 0x0071u, 0x0072u,
    0x0073u, 0x0074u, 0x0075u, 0x0076u, 0x0077u, 0x0078u, 0x0079u, 0x007Au, 0x00E0u,
    0x00E1u, 0x00E2u, 0x00E3u, 0x00E4u, 0x00E5u, 0x00E6u, 0x00E7u, 0x00E8u, 0x00E9u,
    0x00EAu, 0x00EBu, 0x00ECu, 0x00EDu, 0x00EEu, 0x00EFu, 0x00F0u, 0x00F1u, 0x00F2u,
    0x00F3u, 0x00F4u, 0x00F5u, 0x00F6u, 0x00F8u, 0x00F9u, 0x00FAu, 0x00FBu, 0x00FCu,
    0x00FDu, 0x00FEu, 0x00FFu, 0x0101u, 0x0103u, 0x0105u, 0x0107u, 0x0109u, 0x010Bu,
    0x010Du, 0x010Fu, 0x0111u, 0x0113u, 0x0115u, 0x0117u, 0x0119u, 0x011Bu, 0x011Du,
    0x011Fu, 0x0121u, 0x0123u, 0x0125u, 0x0127u, 0x0129u, 0x012Bu, 0x012Du, 0x012Fu,
    0x0131u, 0x0133u, 0x0135u, 0x0137u, 0x013Au, 0x013Cu, 0x013Eu, 0x0140u, 0x0142u,
    0x0144u, 0x0146u, 0x0148u, 0x014Bu, 0x014Du, 0x014Fu, 0x0151u, 0x0153u, 0x0155u,
    0x0157u, 0x0159u, 0x015Bu, 0x015Du, 0x015Fu, 0x0161u, 0x0163u, 0x0165u, 0x0167u,
    0x0169u, 0x016Bu, 0x016Du, 0x016Fu, 0x0171u, 0x0173u, 0x0175u, 0x0177u, 0x017Au,
    0x017Cu, 0x017Eu, 0x0183u, 0x0185u, 0x0188u, 0x018Cu, 0x0192u, 0x0199u, 0x01A1u,
    0x01A3u, 0x01A5u, 0x01A8u, 0x01ADu, 0x01B0u, 0x01B4u, 0x01B6u, 0x01B9u, 0x01BDu,
    0x01C6u, 0x01C9u, 0x01CCu, 0x01CEu, 0x01D0u, 0x01D2u, 0x01D4u, 0x01D6u, 0x01D8u,
    0x01DAu, 0x01DCu, 0x01DFu, 0x01E1u, 0x01E3u, 0x01E5u, 0x01E7u, 0x01E9u, 0x01EBu,
    0x01EDu, 0x01EFu, 0x01F3u, 0x01F5u, 0x01FBu, 0x01FDu, 0x01FFu, 0x0201u, 0x0203u,
    0x0205u, 0x0207u, 0x0209u, 0x020Bu, 0x020Du, 0x020Fu, 0x0211u, 0x0213u, 0x0215u,
    0x0217u, 0x0253u, 0x0254u, 0x0257u, 0x0258u, 0x0259u, 0x025Bu, 0x0260u, 0x0263u,
    0x0268u, 0x0269u, 0x026Fu, 0x0272u, 0x0275u, 0x0283u, 0x0288u, 0x028Au, 0x028Bu,
    0x0292u, 0x03ACu, 0x03ADu, 0x03AEu, 0x03AFu, 0x03B1u, 0x03B2u, 0x03B3u, 0x03B4u,
    0x03B5u, 0x03B6u, 0x03B7u, 0x03B8u, 0x03B9u, 0x03BAu, 0x03BBu, 0x03BCu, 0x03BDu,
    0x03BEu, 0x03BFu, 0x03C0u, 0x03C1u, 0x03C3u, 0x03C4u, 0x03C5u, 0x03C6u, 0x03C7u,
    0x03C8u, 0x03C9u, 0x03CAu, 0x03CBu, 0x03CCu, 0x03CDu, 0x03CEu, 0x03E3u, 0x03E5u,
    0x03E7u, 0x03E9u, 0x03EBu, 0x03EDu, 0x03EFu, 0x0430u, 0x0431u, 0x0432u, 0x0433u,
    0x0434u, 0x0435u, 0x0436u, 0x0437u, 0x0438u, 0x0439u, 0x043Au, 0x043Bu, 0x043Cu,
    0x043Du, 0x043Eu, 0x043Fu, 0x0440u, 0x0441u, 0x0442u, 0x0443u, 0x0444u, 0x0445u,
    0x0446u, 0x0447u, 0x0448u, 0x0449u, 0x044Au, 0x044Bu, 0x044Cu, 0x044Du, 0x044Eu,
    0x044Fu, 0x0451u, 0x0452u, 0x0453u, 0x0454u, 0x0455u, 0x0456u, 0x0457u, 0x0458u,
    0x0459u, 0x045Au, 0x045Bu, 0x045Cu, 0x045Eu, 0x045Fu, 0x0461u, 0x0463u, 0x0465u,
    0x0467u, 0x0469u, 0x046Bu, 0x046Du, 0x046Fu, 0x0471u, 0x0473u, 0x0475u, 0x0477u,
    0x0479u, 0x047Bu, 0x047Du, 0x047Fu, 0x0481u, 0x0491u, 0x0493u, 0x0495u, 0x0497u,
    0x0499u, 0x049Bu, 0x049Du, 0x049Fu, 0x04A1u, 0x04A3u, 0x04A5u, 0x04A7u, 0x04A9u,
    0x04ABu, 0x04ADu, 0x04AFu, 0x04B1u, 0x04B3u, 0x04B5u, 0x04B7u, 0x04B9u, 0x04BBu,
    0x04BDu, 0x04BFu, 0x04C2u, 0x04C4u, 0x04C8u, 0x04CCu, 0x04D1u, 0x04D3u, 0x04D5u,
    0x04D7u, 0x04D9u, 0x04DBu, 0x04DDu, 0x04DFu, 0x04E1u, 0x04E3u, 0x04E5u, 0x04E7u,
    0x04E9u, 0x04EBu, 0x04EFu, 0x04F1u, 0x04F3u, 0x04F5u, 0x04F9u, 0x0561u, 0x0562u,
    0x0563u, 0x0564u, 0x0565u, 0x0566u, 0x0567u, 0x0568u, 0x0569u, 0x056Au, 0x056Bu,
    0x056Cu, 0x056Du, 0x056Eu, 0x056Fu, 0x0570u, 0x0571u, 0x0572u, 0x0573u, 0x0574u,
    0x0575u, 0x0576u, 0x0577u, 0x0578u, 0x0579u, 0x057Au, 0x057Bu, 0x057Cu, 0x057Du,
    0x057Eu, 0x057Fu, 0x0580u, 0x0581u, 0x0582u, 0x0583u, 0x0584u, 0x0585u, 0x0586u,
    0x10D0u, 0x10D1u, 0x10D2u, 0x10D3u, 0x10D4u, 0x10D5u, 0x10D6u, 0x10D7u, 0x10D8u,
    0x10D9u, 0x10DAu, 0x10DBu, 0x10DCu, 0x10DDu, 0x10DEu, 0x10DFu, 0x10E0u, 0x10E1u,
    0x10E2u, 0x10E3u, 0x10E4u, 0x10E5u, 0x10E6u, 0x10E7u, 0x10E8u, 0x10E9u, 0x10EAu,
    0x10EBu, 0x10ECu, 0x10EDu, 0x10EEu, 0x10EFu, 0x10F0u, 0x10F1u, 0x10F2u, 0x10F3u,
    0x10F4u, 0x10F5u, 0x1E01u, 0x1E03u, 0x1E05u, 0x1E07u, 0x1E09u, 0x1E0Bu, 0x1E0Du,
    0x1E0Fu, 0x1E11u, 0x1E13u, 0x1E15u, 0x1E17u, 0x1E19u, 0x1E1Bu, 0x1E1Du, 0x1E1Fu,
    0x1E21u, 0x1E23u, 0x1E25u, 0x1E27u, 0x1E29u, 0x1E2Bu, 0x1E2Du, 0x1E2Fu, 0x1E31u,
    0x1E33u, 0x1E35u, 0x1E37u, 0x1E39u, 0x1E3Bu, 0x1E3Du, 0x1E3Fu, 0x1E41u, 0x1E43u,
    0x1E45u, 0x1E47u, 0x1E49u, 0x1E4Bu, 0x1E4Du, 0x1E4Fu, 0x1E51u, 0x1E53u, 0x1E55u,
    0x1E57u, 0x1E59u, 0x1E5Bu, 0x1E5Du, 0x1E5Fu, 0x1E61u, 0x1E63u, 0x1E65u, 0x1E67u,
    0x1E69u, 0x1E6Bu, 0x1E6Du, 0x1E6Fu, 0x1E71u, 0x1E73u, 0x1E75u, 0x1E77u, 0x1E79u,
    0x1E7Bu, 0x1E7Du, 0x1E7Fu, 0x1E81u, 0x1E83u, 0x1E85u, 0x1E87u, 0x1E89u, 0x1E8Bu,
    0x1E8Du, 0x1E8Fu, 0x1E91u, 0x1E93u, 0x1E95u, 0x1EA1u, 0x1EA3u, 0x1EA5u, 0x1EA7u,
    0x1EA9u, 0x1EABu, 0x1EADu, 0x1EAFu, 0x1EB1u, 0x1EB3u, 0x1EB5u, 0x1EB7u, 0x1EB9u,
    0x1EBBu, 0x1EBDu, 0x1EBFu, 0x1EC1u, 0x1EC3u, 0x1EC5u, 0x1EC7u, 0x1EC9u, 0x1ECBu,
    0x1ECDu, 0x1ECFu, 0x1ED1u, 0x1ED3u, 0x1ED5u, 0x1ED7u, 0x1ED9u, 0x1EDBu, 0x1EDDu,
    0x1EDFu, 0x1EE1u, 0x1EE3u, 0x1EE5u, 0x1EE7u, 0x1EE9u, 0x1EEBu, 0x1EEDu, 0x1EEFu,
    0x1EF1u, 0x1EF3u, 0x1EF5u, 0x1EF7u, 0x1EF9u, 0x1F00u, 0x1F01u, 0x1F02u, 0x1F03u,
    0x1F04u, 0x1F05u, 0x1F06u, 0x1F07u, 0x1F10u, 0x1F11u, 0x1F12u, 0x1F13u, 0x1F14u,
    0x1F15u, 0x1F20u, 0x1F21u, 0x1F22u, 0x1F23u, 0x1F24u, 0x1F25u, 0x1F26u, 0x1F27u,
    0x1F30u, 0x1F31u, 0x1F32u, 0x1F33u, 0x1F34u, 0x1F35u, 0x1F36u, 0x1F37u, 0x1F40u,
    0x1F41u, 0x1F42u, 0x1F43u, 0x1F44u, 0x1F45u, 0x1F51u, 0x1F53u, 0x1F55u, 0x1F57u,
    0x1F60u, 0x1F61u, 0x1F62u, 0x1F63u, 0x1F64u, 0x1F65u, 0x1F66u, 0x1F67u, 0x1F80u,
    0x1F81u, 0x1F82u, 0x1F83u, 0x1F84u, 0x1F85u, 0x1F86u, 0x1F87u, 0x1F90u, 0x1F91u,
    0x1F92u, 0x1F93u, 0x1F94u, 0x1F95u, 0x1F96u, 0x1F97u, 0x1FA0u, 0x1FA1u, 0x1FA2u,
    0x1FA3u, 0x1FA4u, 0x1FA5u, 0x1FA6u, 0x1FA7u, 0x1FB0u, 0x1FB1u, 0x1FD0u, 0x1FD1u,
    0x1FE0u, 0x1FE1u, 0x24D0u, 0x24D1u, 0x24D2u, 0x24D3u, 0x24D4u, 0x24D5u, 0x24D6u,
    0x24D7u, 0x24D8u, 0x24D9u, 0x24DAu, 0x24DBu, 0x24DCu, 0x24DDu, 0x24DEu, 0x24DFu,
    0x24E0u, 0x24E1u, 0x24E2u, 0x24E3u, 0x24E4u, 0x24E5u, 0x24E6u, 0x24E7u, 0x24E8u,
    0x24E9u, 0xFF41u, 0xFF42u, 0xFF43u, 0xFF44u, 0xFF45u, 0xFF46u, 0xFF47u, 0xFF48u,
    0xFF49u, 0xFF4Au, 0xFF4Bu, 0xFF4Cu, 0xFF4Du, 0xFF4Eu, 0xFF4Fu, 0xFF50u, 0xFF51u,
    0xFF52u, 0xFF53u, 0xFF54u, 0xFF55u, 0xFF56u, 0xFF57u, 0xFF58u, 0xFF59u, 0xFF5Au
};

inline constexpr std::array<char32_t, 666> uppers
{
    0x0041u, 0x0042u, 0x0043u, 0x0044u, 0x0045u, 0x0046u, 0x0047u, 0x0048u, 0x0049u,
    0x004Au, 0x004Bu, 0x004Cu, 0x004Du, 0x004Eu, 0x004Fu, 0x0050u, 0x0051u, 0x0052u,
    0x0053u, 0x0054u, 0x0055u, 0x0056u, 0x0057u, 0x0058u, 0x0059u, 0x005Au, 0x00C0u,
    0x00C1u, 0x00C2u, 0x00C3u, 0x00C4u, 0x00C5u, 0x00C6u, 0x00C7u, 0x00C8u, 0x00C9u,
    0x00CAu, 0x00CBu, 0x00CCu, 0x00CDu, 0x00CEu, 0x00CFu, 0x00D0u, 0x00D1u, 0x00D2u,
    0x00D3u, 0x00D4u, 0x00D5u, 0x00D6u, 0x00D8u, 0x00D9u, 0x00DAu, 0x00DBu, 0x00DCu,
    0x00DDu, 0x00DEu, 0x0178u, 0x0100u, 0x0102u, 0x0104u, 0x0106u, 0x0108u, 0x010Au,
    0x010Cu, 0x010Eu, 0x0110u, 0x0112u, 0x0114u, 0x0116u, 0x0118u, 0x011Au, 0x011Cu,
    0x011Eu, 0x0120u, 0x0122u, 0x0124u, 0x0126u, 0x0128u, 0x012Au, 0x012Cu, 0x012Eu,
    0x0049u, 0x0132u, 0x0134u, 0x0136u, 0x0139u, 0x013Bu, 0x013Du, 0x013Fu, 0x0141u,
    0x0143u, 0x0145u, 0x0147u, 0x014Au, 0x014Cu, 0x014Eu, 0x0150u, 0x0152u, 0x0154u,
    0x0156u, 0x0158u, 0x015Au, 0x015Cu, 0x015Eu, 0x0160u, 0x0162u, 0x0164u, 0x0166u,
    0x0168u, 0x016Au, 0x016Cu, 0x016Eu, 0x0170u, 0x0172u, 0x0174u, 0x0176u, 0x0179u,
    0x017Bu, 0x017Du, 0x0182u, 0x0184u, 0x0187u, 0x018Bu, 0x0191u, 0x0198u, 0x01A0u,
    0x01A2u, 0x01A4u, 0x01A7u, 0x01ACu, 0x01AFu, 0x01B3u, 0x01B5u, 0x01B8u, 0x01BCu,
    0x01C4u, 0x01C7u, 0x01CAu, 0x01CDu, 0x01CFu, 0x01D1u, 0x01D3u, 0x01D5u, 0x01D7u,
    0x01D9u, 0x01DBu, 0x01DEu, 0x01E0u, 0x01E2u, 0x01E4u, 0x01E6u, 0x01E8u, 0x01EAu,
    0x01ECu, 0x01EEu, 0x01F1u, 0x01F4u, 0x01FAu, 0x01FCu, 0x01FEu, 0x0200u, 0x0202u,
    0x0204u, 0x0206u, 0x0208u, 0x020Au, 0x020Cu, 0x020Eu, 0x0210u, 0x0212u, 0x0214u,
    0x0216u, 0x0181u, 0x0186u, 0x018Au, 0x018Eu, 0x018Fu, 0x0190u, 0x0193u, 0x0194u,
    0x0197u, 0x0196u, 0x019Cu, 0x019Du, 0x019Fu, 0x01A9u, 0x01AEu, 0x01B1u, 0x01B2u,
    0x01B7u, 0x0386u, 0x0388u, 0x0389u, 0x038Au, 0x0391u, 0x0392u, 0x0393u, 0x0394u,
    0x0395u, 0x0396u, 0x0397u, 0x0398u, 0x0399u, 0x039Au, 0x039Bu, 0x039Cu, 0x039Du,
    0x039Eu, 0x039Fu, 0x03A0u, 0x03A1u, 0x03A3u, 0x03A4u, 0x03A5u, 0x03A6u, 0x03A7u,
    0x03A8u, 0x03A9u, 0x03AAu, 0x03ABu, 0x038Cu, 0x038Eu, 0x038Fu, 0x03E2u, 0x03E4u,
    0x03E6u, 0x03E8u, 0x03EAu, 0x03ECu, 0x03EEu, 0x0410u, 0x0411u, 0x0412u, 0x0413u,
    0x0414u, 0x0415u, 0x0416u, 0x0417u, 0x0418u, 0x0419u, 0x041Au, 0x041Bu, 0x041Cu,
    0x041Du, 0x041Eu, 0x041Fu, 0x0420u, 0x0421u, 0x0422u, 0x0423u, 0x0424u, 0x0425u,
    0x0426u, 0x0427u, 0x0428u, 0x0429u, 0x042Au, 0x042Bu, 0x042Cu, 0x042Du, 0x042Eu,
    0x042Fu, 0x0401u, 0x0402u, 0x0403u, 0x0404u, 0x0405u, 0x0406u, 0x0407u, 0x0408u,
    0x0409u, 0x040Au, 0x040Bu, 0x040Cu, 0x040Eu, 0x040Fu, 0x0460u, 0x0462u, 0x0464u,
    0x0466u, 0x0468u, 0x046Au, 0x046Cu, 0x046Eu, 0x0470u, 0x0472u, 0x0474u, 0x0476u,
    0x0478u, 0x047Au, 0x047Cu, 0x047Eu, 0x0480u, 0x0490u, 0x0492u, 0x0494u, 0x0496u,
    0x0498u, 0x049Au, 0x049Cu, 0x049Eu, 0x04A0u, 0x04A2u, 0x04A4u, 0x04A6u, 0x04A8u,
    0x04AAu, 0x04ACu, 0x04AEu, 0x04B0u, 0x04B2u, 0x04B4u, 0x04B6u, 0x04B8u, 0x04BAu,
    0x04BCu, 0x04BEu, 0x04C1u, 0x04C3u, 0x04C7u, 0x04CBu, 0x04D0u, 0x04D2u, 0x04D4u,
    0x04D6u, 0x04D8u, 0x04DAu, 0x04DCu, 0x04DEu, 0x04E0u, 0x04E2u, 0x04E4u, 0x04E6u,
    0x04E8u, 0x04EAu, 0x04EEu, 0x04F0u, 0x04F2u, 0x04F4u, 0x04F8u, 0x0531u, 0x0532u,
    0x0533u, 0x0534u, 0x0535u, 0x0536u, 0x0537u, 0x0538u, 0x0539u, 0x053Au, 0x053Bu,
    0x053Cu, 0x053Du, 0x053Eu, 0x053Fu, 0x0540u, 0x0541u, 0x0542u, 0x0543u, 0x0544u,
    0x0545u, 0x0546u, 0x0547u, 0x0548u, 0x0549u, 0x054Au, 0x054Bu, 0x054Cu, 0x054Du,
    0x054Eu, 0x054Fu, 0x0550u, 0x0551u, 0x0552u, 0x0553u, 0x0554u, 0x0555u, 0x0556u,
    0x10A0u, 0x10A1u, 0x10A2u, 0x10A3u, 0x10A4u, 0x10A5u, 0x10A6u, 0x10A7u, 0x10A8u,
    0x10A9u, 0x10AAu, 0x10ABu, 0x10ACu, 0x10ADu, 0x10AEu, 0x10AFu, 0x10B0u, 0x10B1u,
    0x10B2u, 0x10B3u, 0x10B4u, 0x10B5u, 0x10B6u, 0x10B7u, 0x10B8u, 0x10B9u, 0x10BAu,
    0x10BBu, 0x10BCu, 0x10BDu, 0x10BEu, 0x10BFu, 0x10C0u, 0x10C1u, 0x10C2u, 0x10C3u,
    0x10C4u, 0x10C5u, 0x1E00u, 0x1E02u, 0x1E04u, 0x1E06u, 0x1E08u, 0x1E0Au, 0x1E0Cu,
    0x1E0Eu, 0x1E10u, 0x1E12u, 0x1E14u, 0x1E16u, 0x1E18u, 0x1E1Au, 0x1E1Cu, 0x1E1Eu,
    0x1E20u, 0x1E22u, 0x1E24u, 0x1E26u, 0x1E28u, 0x1E2Au, 0x1E2Cu, 0x1E2Eu, 0x1E30u,
    0x1E32u, 0x1E34u, 0x1E36u, 0x1E38u, 0x1E3Au, 0x1E3Cu, 0x1E3Eu, 0x1E40u, 0x1E42u,
    0x1E44u, 0x1E46u, 0x1E48u, 0x1E4Au, 0x1E4Cu, 0x1E4Eu, 0x1E50u, 0x1E52u, 0x1E54u,
    0x1E56u, 0x1E58u, 0x1E5Au, 0x1E5Cu, 0x1E5Eu, 0x1E60u, 0x1E62u, 0x1E64u, 0x1E66u,
    0x1E68u, 0x1E6Au, 0x1E6Cu, 0x1E6Eu, 0x1E70u, 0x1E72u, 0x1E74u, 0x1E76u, 0x1E78u,
    0x1E7Au, 0x1E7Cu, 0x1E7Eu, 0x1E80u, 0x1E82u, 0x1E84u, 0x1E86u, 0x1E88u, 0x1E8Au,
    0x1E8Cu, 0x1E8Eu, 0x1E90u, 0x1E92u, 0x1E94u, 0x1EA0u, 0x1EA2u, 0x1EA4u, 0x1EA6u,
    0x1EA8u, 0x1EAAu, 0x1EACu, 0x1EAEu, 0x1EB0u, 0x1EB2u, 0x1EB4u, 0x1EB6u, 0x1EB8u,
    0x1EBAu, 0x1EBCu, 0x1EBEu, 0x1EC0u, 0x1EC2u, 0x1EC4u, 0x1EC6u, 0x1EC8u, 0x1ECAu,
    0x1ECCu, 0x1ECEu, 0x1ED0u, 0x1ED2u, 0x1ED4u, 0x1ED6u, 0x1ED8u, 0x1EDAu, 0x1EDCu,
    0x1EDEu, 0x1EE0u, 0x1EE2u, 0x1EE4u, 0x1EE6u, 0x1EE8u, 0x1EEAu, 0x1EECu, 0x1EEEu,
    0x1EF0u, 0x1EF2u, 0x1EF4u, 0x1EF6u, 0x1EF8u, 0x1F08u, 0x1F09u, 0x1F0Au, 0x1F0Bu,
    0x1F0Cu, 0x1F0Du, 0x1F0Eu, 0x1F0Fu, 0x1F18u, 0x1F19u, 0x1F1Au, 0x1F1Bu, 0x1F1Cu,
    0x1F1Du, 0x1F28u, 0x1F29u, 0x1F2Au, 0x1F2Bu, 0x1F2Cu, 0x1F2Du, 0x1F2Eu, 0x1F2Fu,
    0x1F38u, 0x1F39u, 0x1F3Au, 0x1F3Bu, 0x1F3Cu, 0x1F3Du, 0x1F3Eu, 0x1F3Fu, 0x1F48u,
    0x1F49u, 0x1F4Au, 0x1F4Bu, 0x1F4Cu, 0x1F4Du, 0x1F59u, 0x1F5Bu, 0x1F5Du, 0x1F5Fu,
    0x1F68u, 0x1F69u, 0x1F6Au, 0x1F6Bu, 0x1F6Cu, 0x1F6Du, 0x1F6Eu, 0x1F6Fu, 0x1F88u,
    0x1F89u, 0x1F8Au, 0x1F8Bu, 0x1F8Cu, 0x1F8Du, 0x1F8Eu, 0x1F8Fu, 0x1F98u, 0x1F99u,
    0x1F9Au, 0x1F9Bu, 0x1F9Cu, 0x1F9Du, 0x1F9Eu, 0x1F9Fu, 0x1FA8u, 0x1FA9u, 0x1FAAu,
    0x1FABu, 0x1FACu, 0x1FADu, 0x1FAEu, 0x1FAFu, 0x1FB8u, 0x1FB9u, 0x1FD8u, 0x1FD9u,
    0x1FE8u, 0x1FE9u, 0x24B6u, 0x24B7u, 0x24B8u, 0x24B9u, 0x24BAu, 0x24BBu, 0x24BCu,
    0x24BDu, 0x24BEu, 0x24BFu, 0x24C0u, 0x24C1u, 0x24C2u, 0x24C3u, 0x24C4u, 0x24C5u,
    0x24C6u, 0x24C7u, 0x24C8u, 0x24C9u, 0x24CAu, 0x24CBu, 0x24CCu, 0x24CDu, 0x24CEu,
    0x24CFu, 0xFF21u, 0xFF22u, 0xFF23u, 0xFF24u, 0xFF25u, 0xFF26u, 0xFF27u, 0xFF28u,
    0xFF29u, 0xFF2Au, 0xFF2Bu, 0xFF2Cu, 0xFF2Du, 0xFF2Eu, 0xFF2Fu, 0xFF30u, 0xFF31u,
    0xFF32u, 0xFF33u, 0xFF34u, 0xFF35u, 0xFF36u, 0xFF37u, 0xFF38u, 0xFF39u, 0xFF3Au
};

}

template <typename T, typename = void>
struct is_encoding : std::false_type {};
template<typename T>
struct is_encoding<T, std::void_t<
    typename T::char_type,
    decltype(T::decode(std::declval<typename T::char_type*>(), std::declval<typename T::char_type*>(), std::declval<typename T::char_type&>())),
    decltype(T::encode(std::declval<typename T::char_type>(), std::declval<typename T::char_type&>())),
    decltype(T::next(std::declval<typename T::char_type*>(), std::declval<typename T::char_type*>())),
    decltype(T::count(std::declval<typename T::char_type*>(), std::declval<typename T::char_type*>())),
    decltype(T::max_char_length())
    >
> : std::true_type {};

struct utf8
{
private:
    static constexpr std::array<std::uint32_t, 256> trailing
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
    };

    static constexpr std::array<std::uint32_t, 6> offsets{0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080};
    static constexpr std::array<std::uint8_t, 7> first_bytes{0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

public:
    using char_type = char;

    template<typename InputIt, typename T>
    static constexpr InputIt decode(InputIt begin, InputIt end, T& out) noexcept
    {
        const std::uint32_t trailing_bytes = trailing[static_cast<std::size_t>(*begin)];

        if(begin + trailing_bytes < end)
        {
            out = 0;

            switch(trailing_bytes)
            {
                case 5: out += static_cast<std::uint8_t>(*begin++); out <<= 6; [[fallthrough]];
                case 4: out += static_cast<std::uint8_t>(*begin++); out <<= 6; [[fallthrough]];
                case 3: out += static_cast<std::uint8_t>(*begin++); out <<= 6; [[fallthrough]];
                case 2: out += static_cast<std::uint8_t>(*begin++); out <<= 6; [[fallthrough]];
                case 1: out += static_cast<std::uint8_t>(*begin++); out <<= 6; [[fallthrough]];
                case 0: out += static_cast<std::uint8_t>(*begin++);
            }

            out -= offsets[trailing_bytes];
        }
        else
        {
            begin = end;
            out = 0;
        }

        return begin;
    }

    template<typename OutputIt, typename T>
    static constexpr OutputIt encode(T code, OutputIt out) noexcept
    {
        if((code > 0x0010FFFF) || ((code >= 0xD800) && (code <= 0xDBFF))) //Invalid value
        {
            *out++ = 0;
        }
        else
        {
            std::size_t count{};
            if(code < 0x80)
            {
                count = 1;
            }
            else if(code < 0x800)
            {
                count = 2;
            }
            else if(code < 0x10000)
            {
                count = 3;
            }
            else if(code <= 0x0010FFFF)
            {
                count = 4;
            }

            std::array<std::uint8_t, 4> bytes{};
            switch(count)
            {
                case 4: bytes[3] = static_cast<std::uint8_t>((code | 0x80) & 0xBF); code >>= 6; [[fallthrough]];
                case 3: bytes[2] = static_cast<std::uint8_t>((code | 0x80) & 0xBF); code >>= 6; [[fallthrough]];
                case 2: bytes[1] = static_cast<std::uint8_t>((code | 0x80) & 0xBF); code >>= 6; [[fallthrough]];
                case 1: bytes[0] = static_cast<std::uint8_t> (code | first_bytes[count]);
            }

            for(std::size_t i{}; i < count; ++i)
                *out++ = bytes[i];
        }

        return out;
    }

    template<typename InputIt>
    static constexpr InputIt next(InputIt begin, InputIt end) noexcept
    {
        if(begin == end)
            return end;

        while((*++begin & 0xc0) != 0x80 && begin != end) //Advance as long as it is not the begin of a character code.
        {

        }

        return begin;
    }

    template<typename InputIt>
    static constexpr std::size_t count(InputIt begin, InputIt end) noexcept
    {
        std::size_t count{};

        while(begin != end)
        {
            if((*begin & 0xc0) != 0x80) //Check if it is the begin
                ++count;

            ++begin;
        }

        return count;
    }

    static constexpr std::size_t max_char_length() noexcept
    {
        return 6;
    }
};

struct utf16
{
    using char_type = char16_t;

    template<typename InputIt, typename T>
    static constexpr InputIt decode(InputIt begin, InputIt end, T& out) noexcept
    {
        const std::uint16_t first{*begin++};

        if(first >= 0xD800 && first <= 0xDBFF)
        {
            if(begin < end)
            {
                const std::uint32_t second{*begin++};

                if(second >= 0xDC00 && second <= 0xDFFF)
                {
                    out = static_cast<std::uint32_t>(((first - 0xD800) << 10) + (second - 0xDC00) + 0x0010000);
                }
                else
                {
                    out = 0;
                }
            }
            else
            {
                begin = end;
                out = 0;
            }
        }
        else
        {
            out = first;
        }

        return begin;
    }

    template<typename OutputIt, typename T>
    static constexpr OutputIt encode(T code, OutputIt out) noexcept
    {
        if(code <= 0xFFFF)
        {
            if(code >= 0xD800 && code <= 0xDFFF)
            {
                *out++ = 0;
            }
            else
            {
                *out++ = static_cast<std::uint16_t>(code);
            }
        }
        else if(code > 0x0010FFFF)
        {
            *out++ = 0;
        }
        else
        {
            code -= 0x0010000;
            *out++ = static_cast<std::uint16_t>((code >> 10)     + 0xD800);
            *out++ = static_cast<std::uint16_t>((code & 0x3FFUL) + 0xDC00);
        }

        return out;
    }

    template<typename InputIt>
    static constexpr InputIt next(InputIt begin, InputIt end) noexcept
    {
        if(begin == end)
            return end;

        if((*begin <= 0xD7FF) || (*begin >= 0xE000 && *begin <= 0xFFFD))
        {
            begin += 1;
        }
        else
        {
            begin += 2;
        }

        return begin;
    }

    template<typename InputIt>
    static constexpr std::size_t count(InputIt begin, InputIt end) noexcept
    {
        std::size_t count{};

        while(begin < end)
        {
            if((*begin <= 0xD7FF) || (*begin >= 0xE000 && *begin <= 0xFFFD))
            {
                begin += 1;
            }
            else
            {
                begin += 2;
            }

            ++count;
        }

        return count;
    }

    static constexpr std::size_t max_char_length() noexcept
    {
        return 2;
    }
};

struct utf32
{
    using char_type = char32_t;

    template<typename InputIt, typename T>
    static constexpr InputIt decode(InputIt begin, InputIt, T& out) noexcept
    {
        out = *begin++;
        return begin;
    }

    template<typename OutputIt, typename T>
    static constexpr OutputIt encode(T code, OutputIt out) noexcept
    {
        *out++ = code;
        return out;
    }

    template<typename InputIt>
    static constexpr InputIt next(InputIt begin, InputIt) noexcept
    {
        return ++begin;
    }

    template<typename InputIt>
    static constexpr std::size_t count(InputIt begin, InputIt end) noexcept
    {
        return static_cast<std::size_t>(std::distance(begin, end));
    }

    static constexpr std::size_t max_char_length() noexcept
    {
        return 1;
    }
};

#if __WCHAR_MAX__ == 0xFFFF
struct wide : public utf16
{
    using char_type = wchar_t;
};
#else
struct wide : public utf32
{
    using char_type = wchar_t;
};
#endif

template<typename Input, typename Output>
struct converter
{
    using input_encoding = Input;
    using output_encoding = Output;

    //From input encoding to output encoding:
    template<typename InputIt, typename OutputIt>
    constexpr OutputIt out(InputIt begin, InputIt end, OutputIt output)
    {
        while(begin < end)
        {
            std::uint32_t code{};
            begin  = input_encoding::decode(begin, end, code);
            output = output_encoding::encode(code, output);
        }

        return output;
    }

    template<typename StringIn = std::basic_string<typename input_encoding::char_type>, typename StringOut = std::basic_string<typename output_encoding::char_type>>
    StringOut out(const StringIn& str)
    {
        StringOut output{};
        out(std::begin(str), std::end(str), std::back_inserter(output));
        return output;
    }

    //From output encoding to input encoding:
    template<typename InputIt, typename OutputIt>
    constexpr OutputIt in(InputIt begin, InputIt end, OutputIt output)
    {
        while(begin < end)
        {
            std::uint32_t code{};
            begin  = output_encoding::decode(begin, end, code);
            output = input_encoding::encode(code, output);
        }

        return output;
    }

    template<typename StringIn = std::basic_string<typename input_encoding::char_type>, typename StringOut = std::basic_string<typename output_encoding::char_type>>
    StringOut in(const StringIn& str)
    {
        StringOut output{};
        in(std::begin(str), std::end(str), std::back_inserter(output));
        return output;
    }
};

template<typename Input, typename Output, typename InputIt, typename OutputIt>
constexpr OutputIt convert(InputIt begin, InputIt end, OutputIt output)
{
    converter<Input, Output> conv{};
    conv.out(begin, end, output);

    return output;
}

template<typename Input, typename Output, typename StringIn, typename StringOut = std::basic_string<typename Output::char_type>>
StringOut convert(const StringIn& str)
{
    if constexpr(Output::max_char_length() == 1)
    {
        StringOut out{};
        out.resize(Input::count(std::begin(str), std::end(str)));

        convert<Input, Output>(std::begin(str), std::end(str), std::begin(out));

        return out;
    }
    else
    {
        StringOut out{};
        convert<Input, Output>(std::begin(str), std::end(str), std::back_inserter(out));

        return out;
    }
}

template<typename Encoding, typename InputIt, typename OutputIt>
constexpr OutputIt to_lower(InputIt begin, InputIt end, OutputIt output)
{
    while(begin < end)
    {
        char32_t code{};
        begin = Encoding::decode(begin, end, code);

        auto it = impl::uppers.begin();
        while(it != impl::uppers.end())
            if(*it++ == code)
                break;

        if(it != impl::uppers.end())
            code = *(impl::lowers.begin() + (it - impl::uppers.begin() - 1));

        output = Encoding::encode(code, output);
    }

    return output;
}

template<typename Encoding, typename String = std::basic_string<typename Encoding::char_type>>
String to_lower(const String& str)
{
    String output{};
    output.resize(std::size(str));

    to_lower<Encoding>(std::begin(str), std::end(str), std::begin(output));

    return output;
}

template<typename Encoding, typename InputIt, typename OutputIt>
constexpr OutputIt to_upper(InputIt begin, InputIt end, OutputIt output)
{
    while(begin < end)
    {
        char32_t code{};
        begin = Encoding::decode(begin, end, code);

        auto it = impl::lowers.begin();
        while(it != impl::lowers.end())
            if(*it++ == code)
                break;

        if(it != impl::lowers.end())
            code = *(impl::uppers.begin() + (it - impl::lowers.begin() - 1));

        output = Encoding::encode(code, output);
    }

    return output;
}

template<typename Encoding, typename String = std::basic_string<typename Encoding::char_type>>
String to_upper(const String& str)
{
    String output{};
    output.resize(std::size(str));

    to_upper<Encoding>(std::begin(str), std::end(str), std::begin(output));

    return output;
}

}

#endif
