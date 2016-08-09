#include "TransferFunction.h"

#include <brayns/common/log.h>

namespace
{
    const std::string TF_RED_STRING = "red";
    const std::string TF_GREEN_STRING = "green";
    const std::string TF_BLUE_STRING = "blue";
    const std::string TF_ALPHA_STRING = "alpha";
    const std::string TF_EMISSION_STRING = "emission";
}

namespace brayns
{

TransferFunction::TransferFunction()
{
}

void TransferFunction::clear()
{
    _controlPoints.clear();
    _diffuseColors.clear();
    _emissionIntensities.clear();
}

Vector2fs& TransferFunction::getControlPoints( const TransferFunctionAttribute attribute )
{
    return _controlPoints[ attribute ];
}

void TransferFunction::resample( const size_t sampleSize )
{
    Vector2f range(
        std::numeric_limits<size_t>::max(),
        std::numeric_limits<size_t>::min());
    for( const auto& controlPoints: _controlPoints )
        for( const auto& point: controlPoints.second )
        {
            range.x() = std::min( range.x(), point.x() );
            range.y() = std::max( range.y(), point.x() );
        }
    if( range.x() == range.y() )
        return;

    _diffuseColors.clear();
    _emissionIntensities.clear();

    BRAYNS_DEBUG << "Computing linear deltas between each control points" << std::endl;
    const float step = ( range.y() - range.x() ) / ( sampleSize - 1 );
    std::map< TransferFunctionAttribute, Vector4fs > deltas;
    for( const auto& controlPoints: _controlPoints )
        for( size_t i = 1; i < controlPoints.second.size(); ++i )
        {
            const Vector2f& P0 = controlPoints.second[i - 1];
            const Vector2f& P1 = controlPoints.second[i];
            Vector4f delta = {
                P1.x(),
                step * ((P1.y() - P0.y()) / (P1.x() - P0.x())),
                P1.y(),
                P0.x()
            };
            deltas[controlPoints.first].push_back( delta );
        }

    BRAYNS_DEBUG << "Populating transfer function colors and light emission values" << std::endl;
    size_t xr =0, xg = 0, xb = 0, xa = 0, xe = 0;
    Vector4f color = {
        ( _controlPoints[TF_RED].size() == 0 ? 0.f : _controlPoints[TF_RED][0].y()),
        ( _controlPoints[TF_GREEN].size() == 0 ? 0.f : _controlPoints[TF_GREEN][0].y()),
        ( _controlPoints[TF_BLUE].size() == 0 ? 0.f : _controlPoints[TF_BLUE][0].y()),
        ( _controlPoints[TF_ALPHA].size() == 0 ? 1.f : _controlPoints[TF_ALPHA][0].y()) };

    float emission =
        _controlPoints[TF_EMISSION].size() == 0 ? 0.f : _controlPoints[TF_EMISSION][0].y();

    for( float x = range.x(); x <= range.y(); x += step )
    {
        if( xr < deltas[TF_RED].size() )
        {
            if( x > deltas[TF_RED][xr].w() )
                color.x() += deltas[TF_RED][xr].y();
            if( x >= deltas[TF_RED][xr].x() )
            {
                color.x() = deltas[TF_RED][xr].z();
                ++xr;
            }
        }

        if( xg < deltas[TF_GREEN].size() )
        {
            if( x >= deltas[TF_GREEN][xr].w() )
                color.y() += deltas[TF_GREEN][xg].y();
            if( x >= deltas[TF_GREEN][xg].x() )
            {
                color.y() = deltas[TF_GREEN][xg].z();
                ++xg;
            }
        }

        if( xb < deltas[TF_BLUE].size() )
        {
            color.z() += deltas[TF_BLUE][xb].y();
            if( x >= deltas[TF_BLUE][xb].x() )
            {
                color.z() = deltas[TF_BLUE][xb].z();
                ++xb;
            }
        }

        if( xa < deltas[TF_ALPHA].size() )
        {
            color.a() += deltas[TF_ALPHA][xa].y();
            if( x >= deltas[TF_ALPHA][xa].x() )
            {
                color.a() = deltas[TF_ALPHA][xa].z();
                ++xa;
            }
        }

        if( xe < deltas[TF_EMISSION].size() )
        {
            emission += deltas[TF_EMISSION][xe].y();
            if( x >= deltas[TF_EMISSION][xe].x() )
            {
                emission = deltas[TF_EMISSION][xe].z();
                ++xe;
            }
        }

        color.x() = std::max(0.f, std::min(1.f, color.x()));
        color.y() = std::max(0.f, std::min(1.f, color.y()));
        color.z() = std::max(0.f, std::min(1.f, color.z()));
        color.w() = std::max(0.f, std::min(1.f, color.w()));
        emission = std::max(0.f, std::min(1.f, emission));

        _diffuseColors.push_back( color );
        _emissionIntensities.push_back( emission );
    }
}

std::string TransferFunction::getAttributeAsString( const TransferFunctionAttribute attribute )
{
    switch( attribute )
    {
        case TF_RED:
            return "red"; break;
        case TF_GREEN:
            return "green"; break;
        case TF_BLUE:
            return "blue"; break;
        case TF_ALPHA:
            return "alpha"; break;
        case TF_EMISSION:
            return "emission"; break;
        default:
            return "Undefined";
    }
}

TransferFunctionAttribute TransferFunction::getAttributeFromString( const std::string& attribute )
{
    if( attribute == TF_RED_STRING )
        return TF_RED;
    if( attribute == TF_GREEN_STRING )
        return TF_GREEN;
    if( attribute == TF_BLUE_STRING )
        return TF_BLUE;
    if( attribute == TF_ALPHA_STRING )
        return TF_ALPHA;
    if( attribute == TF_EMISSION_STRING )
        return TF_EMISSION;
    return TF_UNDEFINED;
}


}
